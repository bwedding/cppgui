import { memo, useMemo } from "react";
import { AlertTriangle, AlertCircle } from "lucide-react";
import type { PressureSensor } from '../../types/types.ts';
import { Card } from "@/components/ui/card.tsx";
import { ReadonlySignal } from "@preact/signals-react";
import { useSignals } from "@preact/signals-react/runtime";
import { useAtomValue, Atom } from 'jotai';

interface MetricCardProps {
  title: string;
  unit: string;
  sensorAtom?: Atom<PressureSensor | null>;  
  sensorSignal?: ReadonlySignal<PressureSensor | null>; 
  showArrows?: boolean;
  arrowValue?: number;
}

// Memoized LED bar graph component
const LEDBarGraph = memo(({ sensor }: { sensor: PressureSensor }) => {
  try {
    const meanValue = parseFloat(sensor.MeanValue);
    const minValue = parseFloat(sensor.MinValue);
    const maxValue = parseFloat(sensor.MaxValue);
    
    // Check if any values are NaN
    if (isNaN(meanValue) || isNaN(minValue) || isNaN(maxValue)) {
      return (
        <div className="w-full flex items-center gap-1.5">
          <span className="text-[clamp(0.6rem,1vw,0.75rem)] text-neutral-500">--</span>
          <div className="flex-1 flex gap-[2px] h-4 p-1 border border-gray-800 bg-neutral-950 rounded">
            {[...Array(20)].map((_, i) => (
              <div key={i} className="flex-1 h-full bg-neutral-700" />
            ))}
          </div>
          <span className="text-[clamp(0.6rem,1vw,0.75rem)] text-neutral-500">--</span>
        </div>
      );
    }
    
    // Round to nearest 5mmHg for the display range
    const roundTo5 = (value: number) => Math.ceil(Math.abs(value) / 5) * 5;
    const displayRange = Math.max(roundTo5(minValue), roundTo5(maxValue), 15); // At least ±15
    
    const segments = 20;
    const range = displayRange * 2; // Total range is ±displayRange
    const segmentWidth = range / segments;
    
    const getSegmentColor = (segmentIndex: number) => {
      const segmentValue = -displayRange + (segmentIndex * segmentWidth);
      
      // If segment is in negative range (before zero)
      if (segmentValue < 0) {
        // Color red if we have a negative min value and this segment is in its range
        return (minValue < 0 && segmentValue >= minValue) ? 'bg-red-500' : 'bg-neutral-700';
      } 
      // If segment is in positive range (after zero)
      else {
        // Color green if this segment is within the mean value range
        return (segmentValue <= meanValue) ? 'bg-green-500' : 'bg-neutral-700';
      }
    };

    return (
      <div style={{ textSizeAdjust: 'none' }} className="w-full flex items-center gap-1.5">
        <span className="text-[clamp(0.6rem,1vw,0.75rem)] text-neutral-500">{-displayRange}</span>
        <div className="flex-1 flex gap-[2px] h-5 p-1 border border-gray-800 bg-neutral-950 rounded">
          {[...Array(segments)].map((_, i) => (
            <div key={i} className={`flex-1 h-full ${getSegmentColor(i)}`} />
          ))}
        </div>
        <span className="text-[clamp(0.6rem,1vw,0.75rem)] text-neutral-500">{displayRange}</span>
      </div>
    );
  } catch (error) {
    console.error('Error in LEDBarGraph:', error);
    return null;
  }
}, (prevProps, nextProps) => {
  // Custom comparison to prevent unnecessary re-renders
  return (
    prevProps.sensor.MeanValue === nextProps.sensor.MeanValue &&
    prevProps.sensor.MinValue === nextProps.sensor.MinValue &&
    prevProps.sensor.MaxValue === nextProps.sensor.MaxValue
  );
});

// Memoized status bar component
const StatusBar = memo(({ status }: { status?: "normal" | "warning" | "error" }) => {
  if (!status || status === "normal") return null;

  return (
    <div className={`absolute top-0 left-0 right-0 rounded-t-lg ${status === "error" ? "bg-red-500" : "bg-orange-400"}`}>
      <div className="h-4 flex items-center justify-center py-0.5">
        {status === "warning" ? (
          <AlertTriangle className="w-4 h-4 text-black" />
        ) : (
          <AlertCircle className="w-4 h-4 text-black" />
        )}
      </div>
    </div>
  );
});

// Memoized arrows component
const Arrows = memo(({ showArrows, arrowValue }: { showArrows: boolean; arrowValue: number }) => {
  if (!showArrows) return null;
  
  return (
    <div className="flex items-center gap-1 text-gray-400 text-sm">
      <span className="min-h-4">{arrowValue === -1 ? "\u00A0" : "←"}</span>
      <span className="min-h-4">{arrowValue === -1 ? "\u00A0" : arrowValue}</span>
      <span className="min-h-4">{arrowValue === -1 ? "\u00A0" : "→"}</span>
    </div>
  );
});

// Memoized value display component
const ValueDisplay = memo(({ sensor }: { sensor: PressureSensor | null }) => {
  if (!sensor) {
    return (
      <div className="flex-1 flex flex-col items-center justify-center min-h-0">
        <div className="text-[clamp(1rem,4vw,2.5rem)] font-bold text-white">--</div>
        <div className="text-[clamp(0.75rem,1.5vw,0.875rem)] text-gray-400 mt-2">mmHg</div>
      </div>
    );
  }

  return (
    <div className="flex-1 flex flex-col items-center justify-center min-h-0">
      <div className="text-[clamp(1rem,4vw,2.5rem)] font-bold text-white">{Number(sensor.MeanValue).toFixed(1)}</div>
      <div className="text-[clamp(0.75rem,1.5vw,0.875rem)] text-gray-400 mt-2">mmHg</div>
    </div>
  );
});

const HiLoPressureCard = memo(({
  title,
  unit,
  sensorAtom,
  sensorSignal,
  showArrows = false,
  arrowValue = 15,
}: MetricCardProps) => {
  // Enable signal reactivity
  useSignals();
  
  // Support both atom and signal for backward compatibility
  const sensorFromAtom = sensorAtom ? useAtomValue(sensorAtom) : null;
  const sensor = sensorSignal?.value || sensorFromAtom;
  
  // Status is determined based on both mean value and min/max pressure range
  const getStatus = (): "normal" | "warning" | "error" => {
    if (!sensor) return "normal";

    try {
      const meanValue = parseFloat(sensor.MeanValue);
      const minValue = parseFloat(sensor.MinValue);
      
      // If the data is invalid, display as normal
      if (isNaN(meanValue) || isNaN(minValue)) return "normal";

      // For demo purposes, we use static values to determine warning/error
      if (minValue < -10) return "warning";
      if (minValue < -15) return "error";
      
      return "normal";
    } catch (error) {
      // If there's an error parsing, display as normal
      return "normal";
    }
  };

  // Memoize the title since it rarely changes
  const Title = useMemo(() => (
    <div className="text-[clamp(0.75rem,1.5vw,0.875rem)] font-medium text-gray-300 text-center min-h-[clamp(40px,10vw,60px)] flex items-center justify-center">
      {title}
    </div>
  ), [title]);

  // Calculate status
  const status = getStatus();

  return (
    <Card className="relative bg-neutral-900 border border-gray-800 p-4 flex flex-col items-center h-[220px] overflow-hidden w-[140px]">
      <StatusBar status={status} />
      {Title}
      <ValueDisplay sensor={sensor} />
      <Arrows showArrows={showArrows} arrowValue={arrowValue} />
      <LEDBarGraph sensor={sensor || { MeanValue: "0", MinValue: "0", MaxValue: "0" }} />
    </Card>
  );
}, (prevProps, nextProps) => {
  // Custom comparison function to prevent unnecessary re-renders
  return (
    prevProps.title === nextProps.title &&
    prevProps.unit === nextProps.unit &&
    prevProps.sensorAtom === nextProps.sensorAtom &&
    prevProps.sensorSignal === nextProps.sensorSignal &&
    prevProps.showArrows === nextProps.showArrows &&
    prevProps.arrowValue === nextProps.arrowValue
  );
});

// Add display names for better debugging
LEDBarGraph.displayName = 'LEDBarGraph';
StatusBar.displayName = 'StatusBar';
Arrows.displayName = 'Arrows';
ValueDisplay.displayName = 'ValueDisplay';
HiLoPressureCard.displayName = 'HiLoPressureCard';

export default HiLoPressureCard;