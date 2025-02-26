// MiniChart.tsx
import { useAtomValue, PrimitiveAtom } from 'jotai';
import { useRef, useEffect, useMemo, memo } from 'react';
import { createChart, ColorType, BaselineSeries, IChartApi, ISeriesApi } from 'lightweight-charts';

interface HistoryData {
  times: number[];
  oscillations: number[];
  mean: number | null;
}

interface MiniChartProps {
  historyAtom: PrimitiveAtom<HistoryData>;
  baselineValue?: number; // Optional baseline value, defaults to 0
  redHi?: boolean; // Optional flag to control if high values should be red (true) or low values should be red (false)
}

const calculateEMA = (data: number[], smoothingFactor: number = 0.2): number[] => {
  if (data.length === 0) return [];
  
  const ema: number[] = [data[0]]; // First value is same as input
  
  for (let i = 1; i < data.length; i++) {
    const currentValue = data[i];
    const previousEMA = ema[i - 1];
    const currentEMA = (currentValue * smoothingFactor) + (previousEMA * (1 - smoothingFactor));
    ema.push(currentEMA);
  }
  
  return ema;
};

const MiniChart = memo(({ historyAtom, baselineValue = 0, redHi = false }: MiniChartProps) => {
  const chartContainerRef = useRef<HTMLDivElement>(null);
  const chartRef = useRef<IChartApi | null>(null);
  const seriesRef = useRef<ISeriesApi<"Baseline"> | null>(null);
  const historicalData = useAtomValue(historyAtom);

  // Create chart when component mounts and clean up on unmount
  useEffect(() => {
    if (chartContainerRef.current) {
      // Create chart
      const chart = createChart(chartContainerRef.current, {
        layout: {
          background: { type: ColorType.Solid, color: 'transparent' },
          textColor: 'rgba(255, 255, 255, 0.9)',
        },
        width: chartContainerRef.current.clientWidth,
        height: chartContainerRef.current.clientHeight,
        rightPriceScale: {
          visible: false, // Hide the price scale
        },
        timeScale: {
          visible: false, // Hide the time scale
        },
        grid: {
          vertLines: { visible: false },
          horzLines: { visible: false },
        },
        crosshair: {
          horzLine: { visible: false },
          vertLine: { visible: false },
        },
        handleScale: false, // Disable zooming
        handleScroll: false, // Disable scrolling
        watermark: {
          visible: false, // Remove the TradingView logo
        },
      });

      // Add baseline series
      const baselineSeries = chart.addSeries(BaselineSeries, {
        baseValue: { type: 'price', price: baselineValue },
        topLineColor: redHi ? 'rgba(239, 83, 80, 1)' : 'rgba(38, 166, 154, 1)', // Red for high values if redHi is true, green otherwise
        topFillColor1: redHi ? 'rgba(239, 83, 80, 0.28)' : 'rgba(38, 166, 154, 0.28)',
        topFillColor2: redHi ? 'rgba(239, 83, 80, 0.05)' : 'rgba(38, 166, 154, 0.05)',
        bottomLineColor: redHi ? 'rgba(38, 166, 154, 1)' : 'rgba(239, 83, 80, 1)', // Green for low values if redHi is true, red otherwise
        bottomFillColor1: redHi ? 'rgba(38, 166, 154, 0.05)' : 'rgba(239, 83, 80, 0.05)',
        bottomFillColor2: redHi ? 'rgba(38, 166, 154, 0.28)' : 'rgba(239, 83, 80, 0.28)',
        lineWidth: 1,
        lastValueVisible: false, // Hide the last value label
      });

      // Store references
      chartRef.current = chart;
      seriesRef.current = baselineSeries;

      // Handle resize
      const handleResize = () => {
        if (chartContainerRef.current && chartRef.current) {
          chartRef.current.applyOptions({
            width: chartContainerRef.current.clientWidth,
            height: chartContainerRef.current.clientHeight,
          });
        }
      };

      window.addEventListener('resize', handleResize);

      // Clean up
      return () => {
        window.removeEventListener('resize', handleResize);
        if (chartRef.current) {
          chartRef.current.remove();
          chartRef.current = null;
          seriesRef.current = null;
        }
      };
    }
  }, [baselineValue, redHi]); // Recreate chart if baselineValue or redHi changes

  // Update the data when historicalData changes
  useEffect(() => {
    if (!historicalData || !historicalData.oscillations || historicalData.oscillations.length === 0 || !seriesRef.current) {
      return;
    }

    // Calculate smoothed values
    const smoothedValues = calculateEMA(historicalData.oscillations);
    
    // Format data for lightweight-charts
    const chartData = smoothedValues.map((value, index) => ({
      time: historicalData.times[index] / 1000, // Convert to seconds
      value: value,
    }));

    // Update chart data
    seriesRef.current.setData(chartData);
    
    // Fit content to display all data
    if (chartRef.current) {
      chartRef.current.timeScale().fitContent();
    }
  }, [historicalData]);

  // Apply baseline value if it changes
  useEffect(() => {
    if (seriesRef.current) {
      seriesRef.current.applyOptions({
        baseValue: { type: 'price', price: baselineValue },
      });
    }
  }, [baselineValue]);

  return (
    <div style={{ height: '25px', width: '100%', marginTop: '2px', marginBottom: '2px', position: 'relative' }} ref={chartContainerRef} />
  );
});

// Add display name for better debugging
MiniChart.displayName = 'MiniChart';

export default MiniChart;