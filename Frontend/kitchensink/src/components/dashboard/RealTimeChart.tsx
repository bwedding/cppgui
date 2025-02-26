import { leftHeartAtom, rightHeartAtom, leftHeartHistoryAtom, rightHeartHistoryAtom } from '@/lib/datastore';
import React, { useMemo, useEffect, useRef } from 'react';
import { atom, useAtomValue, useSetAtom } from 'jotai';
import { createChart, ColorType, BarSeries, BarData, IChartApi, ISeriesApi } from 'lightweight-charts';

const MAX_HISTORY_MS = 15000; // 15 seconds in milliseconds

// Memoized pressure atom creation
const createPressureAtom = (showingLeftHeart: boolean) => 
  atom((get) => {
    const heart = get(showingLeftHeart ? leftHeartAtom : rightHeartAtom);
    if (!heart?.IntPressure) return null;
    return {
      primary: parseFloat(heart.IntPressure.MeanValue),
      high: parseFloat(heart.IntPressureMax),
      low: parseFloat(heart.IntPressureMin)
    };
  });

// Memoized historical data update function
const updateHistoricalData = (pressure: any, prev: any) => {
  if (!pressure) return prev;
  
  const now = new Date();
  const timestamp = now.getTime();
  
  // Initialize arrays if they don't exist
  const currentTimes = prev.times || [];
  const currentMeans = prev.means || [];
  const currentHighs = prev.highs || [];
  const currentLows = prev.lows || [];
  const currentData = prev.data || [];
  
  // Get the previous mean value to use as "open" or use current mean if it's the first data point
  const prevMean = currentMeans.length > 0 ? currentMeans[currentMeans.length - 1] : pressure.primary;
  
  // Add new data points
  const newTimes = [...currentTimes, timestamp];
  const newMeans = [...currentMeans, pressure.primary];
  const newHighs = [...currentHighs, pressure.high];
  const newLows = [...currentLows, pressure.low];
  
  // Create new bar data point for lightweight-charts
  const newBarData: BarData = {
    time: timestamp / 1000, // Convert to seconds for lightweight-charts
    open: prevMean,
    high: pressure.high,
    low: pressure.low,
    close: pressure.primary,
    color: pressure.low < 0 ? '#ef5350' : '#26a69a' // Red if negative, green otherwise
  };
  
  const newData = [...currentData, newBarData];
  
  // Remove data points older than MAX_HISTORY_MS
  const cutoffTime = now.getTime() - MAX_HISTORY_MS;
  const startIndex = newTimes.findIndex(time => time >= cutoffTime);
  
  if (startIndex > 0) {
    return {
      times: newTimes.slice(startIndex),
      means: newMeans.slice(startIndex),
      highs: newHighs.slice(startIndex),
      lows: newLows.slice(startIndex),
      data: newData.slice(startIndex)
    };
  }
  
  return {
    times: newTimes,
    means: newMeans,
    highs: newHighs,
    lows: newLows,
    data: newData
  };
};

const PressureChart = React.memo(({ showingLeftHeart }: { showingLeftHeart: boolean }) => {
  // Memoize the pressure atom to prevent recreation on every render
  const pressureAtom = useMemo(() => createPressureAtom(showingLeftHeart), [showingLeftHeart]);
  const pressure = useAtomValue(pressureAtom);
  
  // Get the correct history atom based on which heart we're showing
  const historyAtom = showingLeftHeart ? leftHeartHistoryAtom : rightHeartHistoryAtom;
  const historicalData = useAtomValue(historyAtom);
  const setHistoricalData = useSetAtom(historyAtom);

  // Refs for chart and series
  const chartContainerRef = useRef<HTMLDivElement>(null);
  const chartRef = useRef<IChartApi | null>(null);
  const seriesRef = useRef<ISeriesApi<"Bar"> | null>(null);
  const legendRef = useRef<HTMLDivElement | null>(null);

  // Update historical data when pressure changes
  useEffect(() => {
    if (pressure) {
      setHistoricalData(prev => updateHistoricalData(pressure, prev));
    }
  }, [pressure, setHistoricalData]);

  // Create chart on mount and clean up on unmount
  useEffect(() => {
    if (chartContainerRef.current) {
      // Set position relative to contain the legend properly
      chartContainerRef.current.style.position = 'relative';
      
      // Create legend element
      const legendElement = document.createElement('div');
      legendElement.style.cssText = `
        position: absolute;
        left: 12px;
        top: 12px;
        z-index: 2;
        font-size: 14px;
        font-family: sans-serif;
        line-height: 18px;
        font-weight: 300;
        color: rgba(255, 255, 255, 0.9);
        background-color: rgba(0, 0, 0, 0.2);
        padding: 8px;
        border-radius: 4px;
        pointer-events: none;
      `;
      chartContainerRef.current.appendChild(legendElement);
      legendRef.current = legendElement;

      const chart = createChart(chartContainerRef.current, {
        layout: {
          background: { type: ColorType.Solid, color: 'transparent' },
          textColor: 'rgba(255, 255, 255, 0.9)',
        },
        grid: {
          vertLines: { color: 'rgba(197, 203, 206, 0.3)' },
          horzLines: { color: 'rgba(197, 203, 206, 0.3)' },
        },
        timeScale: {
          timeVisible: true,
          secondsVisible: true,
        },
        rightPriceScale: {
          borderColor: 'rgba(197, 203, 206, 0.8)',
          scaleMargins: {
            top: 0.3, // leave space for the legend
            bottom: 0.15,
          },
        },
        width: chartContainerRef.current.clientWidth,
        height: chartContainerRef.current.clientHeight,
        attributionLogo: false,
        watermark: {
          visible: false, // Remove the TradingView logo
        },
      });

      const handleResize = () => {
        if (chartContainerRef.current) {
          chart.applyOptions({
            width: chartContainerRef.current.clientWidth,
            height: chartContainerRef.current.clientHeight,
          });
        }
      };

      // Add bar series
      const barSeries = chart.addSeries(BarSeries, {
        thinBars: false,
      });

      // Store refs
      chartRef.current = chart;
      seriesRef.current = barSeries;

      // Add resize listener
      window.addEventListener('resize', handleResize);

      return () => {
        window.removeEventListener('resize', handleResize);
        if (legendRef.current && chartContainerRef.current) {
          chartContainerRef.current.removeChild(legendRef.current);
        }
        chart.remove();
        chartRef.current = null;
        seriesRef.current = null;
        legendRef.current = null;
      };
    }
  }, [showingLeftHeart]);

  // Function to format values with 2 decimal places
  const formatValue = (value: number): string => {
    return (Math.round(value * 100) / 100).toFixed(2);
  };

  // Update legend with the current data
  const updateLegend = () => {
    if (legendRef.current && pressure) {
      const title = `${showingLeftHeart ? 'Left' : 'Right'} Atrial Pressure`;
      const meanValue = formatValue(pressure.primary);
      const minMaxValues = `Min: ${formatValue(pressure.low)} / Max: ${formatValue(pressure.high)}`;
      
      legendRef.current.innerHTML = `
        <div style="font-size: 16px; margin: 4px 0px; font-weight: bold;">${title}</div>
        <div style="font-size: 15px; margin: 4px 0px;">${meanValue} mmHg</div>
        <div>${minMaxValues}</div>
      `;
    }
  };

  // Update data when historicalData changes
  useEffect(() => {
    if (seriesRef.current && historicalData?.data?.length > 0) {
      seriesRef.current.setData(historicalData.data);
      
      if (chartRef.current) {
        // Fit content to show all data
        chartRef.current.timeScale().fitContent();
      }
      
      // Update the legend
      updateLegend();
    }
  }, [historicalData, showingLeftHeart]);

  // Update legend when pressure changes
  useEffect(() => {
    updateLegend();
  }, [pressure, showingLeftHeart]);

  return (
    <div className="w-full h-full" ref={chartContainerRef} />
  );
}, (prevProps, nextProps) => prevProps.showingLeftHeart === nextProps.showingLeftHeart);

PressureChart.displayName = 'PressureChart';

export default PressureChart;