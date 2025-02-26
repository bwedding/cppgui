import { useMemo, memo } from "react";
import { Card } from "@/components/ui/card.tsx";
import { AlertTriangle, AlertCircle } from "lucide-react";
import MiniChart from "./MiniChart.tsx";
import { PrimitiveAtom } from 'jotai';
import { HistoryData } from '@/types/history.ts';

interface MetricCardProps {
  title: string;
  value: number;
  unit: string;
  status?: "normal" | "warning" | "error";
  showArrows?: boolean;
  arrowValue?: number;
  showChart?: boolean;
  historyAtom?: PrimitiveAtom<HistoryData>;
  baselineValue?: number; // Optional baseline value for MiniChart
  redHi?: boolean; // Optional flag to determine if high values should be red
}

// Memoized value display component
const ValueDisplay = memo(({ value, unit }: { value: number; unit: string }) => (
  <div style={{ textSizeAdjust: 'none' }} className="flex-1 flex flex-col items-center justify-center min-h-0">
    <div className="text-[clamp(1rem,4vw,2.5rem)] font-bold text-white">{Number(value).toFixed(1)}</div>
    <div className="text-[clamp(0.75rem,1.5vw,0.875rem)] text-gray-400 mt-2">{unit}</div>
  </div>
));

// Memoized status bar component
const StatusBar = memo(({ status }: { status: "normal" | "warning" | "error" }) => {
  if (status === "normal") return null;

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
    <div className="flex items-center gap-1 text-gray-400 text-[clamp(0.75rem,1.2vw,0.875rem)]">
      <span className="min-h-4">{arrowValue === -1 ? "\u00A0" : "←"}</span>
      <span className="min-h-4">{arrowValue === -1 ? "\u00A0" : arrowValue}</span>
      <span className="min-h-4">{arrowValue === -1 ? "\u00A0" : "→"}</span>
    </div>
  );
});

// Memoized chart component
const ChartSection = memo(({ 
  showChart, 
  historyAtom, 
  baselineValue = 15, 
  redHi = false 
}: { 
  showChart: boolean; 
  historyAtom?: PrimitiveAtom<HistoryData>;
  baselineValue?: number;
  redHi?: boolean;
}) => {
  if (!showChart || !historyAtom) return null;
  
  return (
    <div className="w-full h-[clamp(1.5rem,3vw,2rem)] p-1 border border-neutral-800 bg-neutral-950 rounded overflow-hidden whitespace-nowrap text-ellipsis"
      style={{ boxShadow: 'inset 0 0 10px rgba(0, 0, 0, 0.5)' }}>
      <MiniChart historyAtom={historyAtom} baselineValue={baselineValue} redHi={redHi} />
    </div>
  );
});

const MetricCard = memo(({
  title,
  value,
  unit,
  status = "normal",
  showArrows = false,
  arrowValue = 15,
  historyAtom,
  showChart = true,
  baselineValue = 15,
  redHi = false,
}: MetricCardProps) => {
  // Memoize the title since it rarely changes
  const Title = useMemo(() => (
    <div className="text-[clamp(0.75rem,1.5vw,0.875rem)] font-medium text-gray-300 text-center min-h-[clamp(40px,10vw,60px)] flex items-center justify-center">
      {title}
    </div>
  ), [title]);

  return (
    <Card className="relative bg-neutral-900 border border-neutral-700 p-4 flex flex-col items-center h-[220px] overflow-hidden w-[140px]" style={{ boxShadow: 'inset 0 0 10px rgba(0, 0, 0, 0.5)' }}>
      <StatusBar status={status} />
      {Title}
      <ValueDisplay value={value} unit={unit} />
      <Arrows showArrows={showArrows} arrowValue={arrowValue} />
      <ChartSection 
        showChart={showChart} 
        historyAtom={historyAtom} 
        baselineValue={baselineValue}
        redHi={redHi}
      />
    </Card>
  );
}, (prevProps, nextProps) => {
  // Custom comparison function to prevent unnecessary re-renders
  return (
    prevProps.title === nextProps.title &&
    prevProps.value === nextProps.value &&
    prevProps.unit === nextProps.unit &&
    prevProps.status === nextProps.status &&
    prevProps.showArrows === nextProps.showArrows &&
    prevProps.arrowValue === nextProps.arrowValue &&
    prevProps.showChart === nextProps.showChart &&
    prevProps.historyAtom === nextProps.historyAtom &&
    prevProps.baselineValue === nextProps.baselineValue &&
    prevProps.redHi === nextProps.redHi
  );
});

// Add display names for better debugging
ValueDisplay.displayName = 'ValueDisplay';
StatusBar.displayName = 'StatusBar';
Arrows.displayName = 'Arrows';
ChartSection.displayName = 'ChartSection';
MetricCard.displayName = 'MetricCard';

export default MetricCard;
