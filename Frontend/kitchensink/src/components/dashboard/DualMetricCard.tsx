import { memo } from "react";
import { Card } from "@/components/ui/card.tsx";
import { AlertTriangle, AlertCircle } from "lucide-react";
import MiniChart from "./MiniChart.tsx";
import { PrimitiveAtom } from 'jotai';
import { HistoryData } from '@/types/history.ts';

interface MetricData {
  title: string;
  value: number;
  unit: string;
  status?: "normal" | "warning" | "error";
  historyAtom?: PrimitiveAtom<HistoryData>;
}

interface DualMetricCardProps {
  topMetric: MetricData;
  bottomMetric: MetricData;
  baselineValue?: number; // Optional baseline value for both MiniCharts
  redHi?: boolean; // Optional flag to determine if high values should be red for both charts
}

// Memoized status icon component
const StatusIcon = memo(({ status }: { status: "normal" | "warning" | "error" }) => {
  if (status === "normal") return null;

  return (
    <div className={`${status === "error" ? "text-red-500" : "text-orange-400"}`}>
      {status === "warning" ? (
        <AlertTriangle className="w-3 h-3" />
      ) : (
        <AlertCircle className="w-3 h-3" />
      )}
    </div>
  );
});

// Memoized status bar component
const StatusBar = memo(({ topStatus, bottomStatus }: { 
  topStatus?: "normal" | "warning" | "error";
  bottomStatus?: "normal" | "warning" | "error";
}) => {
  const getCombinedStatus = () => {
    const statuses = [topStatus, bottomStatus];
    if (statuses.includes("error")) return "error";
    if (statuses.includes("warning")) return "warning";
    return "normal";
  };

  const combinedStatus = getCombinedStatus();
  if (combinedStatus === "normal") return null;

  return (
    <div className={`absolute top-0 left-0 right-0 rounded-t-lg ${combinedStatus === "error" ? "bg-red-500" : "bg-orange-400"}`}>
      <div className="h-4 flex items-center justify-center py-0.5">
        {combinedStatus === "warning" ? (
          <AlertTriangle className="w-4 h-4 text-black" />
        ) : (
          <AlertCircle className="w-4 h-4 text-black" />
        )}
      </div>
    </div>
  );
});

// Memoized metric value component
const MetricValue = memo(({ value, unit }: { value: number; unit: string }) => (
  <div className="flex items-baseline justify-center gap-1 h-5 pt-1 mb-2">
    <div className="text-[clamp(0.875rem,2.5vw,1rem)] font-bold text-white">
      {Number(value).toFixed(1)}
    </div>
    <div className="text-[clamp(0.6rem,1vw,0.65rem)] text-gray-400">{unit}</div>
  </div>
));

// Memoized chart component
const ChartSection = memo(({ 
  historyAtom, 
  baselineValue = 15, 
  redHi = false 
}: { 
  historyAtom?: PrimitiveAtom<HistoryData>;
  baselineValue?: number;
  redHi?: boolean;
}) => {
  if (!historyAtom) return null;
  
  return (
    <div className="w-full \-[clamp(1rem,3vw,2rem)] p-1 border border-neutral-800 bg-neutral-950 rounded overflow-hidden whitespace-nowrap text-ellipsis"
      style={{ boxShadow: 'inset 0 0 10px rgba(0, 0, 0, 0.5)' }}>
      <MiniChart historyAtom={historyAtom} baselineValue={baselineValue} redHi={redHi} />
    </div>
  );
});

// Memoized metric section component
const MetricSection = memo(({ 
  metric, 
  baselineValue, 
  redHi 
}: { 
  metric: MetricData; 
  baselineValue?: number;
  redHi?: boolean;
}) => (
  <div className="flex flex-col items-center w-full h-[clamp(70px,22vw,75px)]">
    <div className="flex items-center justify-center my-1 gap-2 text-[clamp(0.6rem,1.2vw,0.7rem)] font-medium text-gray-300 h-4 text-center w-full">
      {metric.title}
    </div>
    <MetricValue value={metric.value} unit={metric.unit} />
    <ChartSection 
      historyAtom={metric.historyAtom} 
      baselineValue={baselineValue}
      redHi={redHi}
    />
  </div>
));

const DualMetricCard = memo(({
  topMetric,
  bottomMetric,
  baselineValue = 15,
  redHi = false,
}: DualMetricCardProps) => {
  return (
    <Card className="relative bg-neutral-900 border border-gray-800 p-2 flex flex-col items-center h-[220px] overflow-hidden w-[140px]">
      <StatusBar topStatus={topMetric.status} bottomStatus={bottomMetric.status} />
      <div className="flex-1 flex flex-col items-center justify-between w-full gap-1 mt-4">
        <MetricSection 
          metric={topMetric} 
          baselineValue={baselineValue}
          redHi={redHi}
        />
        <div className="w-full border-t border-gray-800" />
        <MetricSection 
          metric={bottomMetric} 
          baselineValue={baselineValue}
          redHi={redHi}
        />
      </div>
    </Card>
  );
}, (prevProps, nextProps) => {
  // Custom comparison function to prevent unnecessary re-renders
  return (
    prevProps.topMetric.title === nextProps.topMetric.title &&
    prevProps.topMetric.value === nextProps.topMetric.value &&
    prevProps.topMetric.unit === nextProps.topMetric.unit &&
    prevProps.topMetric.status === nextProps.topMetric.status &&
    prevProps.topMetric.historyAtom === nextProps.topMetric.historyAtom &&
    prevProps.bottomMetric.title === nextProps.bottomMetric.title &&
    prevProps.bottomMetric.value === nextProps.bottomMetric.value &&
    prevProps.bottomMetric.unit === nextProps.bottomMetric.unit &&
    prevProps.bottomMetric.status === nextProps.bottomMetric.status &&
    prevProps.bottomMetric.historyAtom === nextProps.bottomMetric.historyAtom &&
    prevProps.baselineValue === nextProps.baselineValue &&
    prevProps.redHi === nextProps.redHi
  );
});

// Add display names for better debugging
StatusIcon.displayName = 'StatusIcon';
StatusBar.displayName = 'StatusBar';
MetricValue.displayName = 'MetricValue';
ChartSection.displayName = 'ChartSection';
MetricSection.displayName = 'MetricSection';
DualMetricCard.displayName = 'DualMetricCard';

export default DualMetricCard;
