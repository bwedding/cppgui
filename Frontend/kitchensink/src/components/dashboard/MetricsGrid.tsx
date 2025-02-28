import MetricCard from "./MetricCard.tsx";
import HeartControl from "./HeartControl.tsx";
import OperationModeToggle from "./OperationModeToggle.tsx";
import React, { useRef, useEffect, useMemo } from 'react';
import PhysiologicalControl from "./PhysiologicalControl.tsx";
import DualMetricCard from "./DualMetricCard.tsx";
import { useSetAtom } from 'jotai';
import { leftHeartSignal, rightHeartSignal } from '@/lib/datastore.ts';
import { PressureSensor } from '@/types/types.ts';
import HiLoPressureCard from "./HiLoPressureCard.tsx";
import PressureChart from "./RealTimeChart.tsx";
import { useSignals } from "@preact/signals-react/runtime";
import { computed } from "@preact/signals-react";

import { 
  leftOutflowPressureHistoryAtom,
  rightOutflowPressureHistoryAtom,
  leftThermistorTempHistoryAtom,
  rightThermistorTempHistoryAtom,
  leftSensorTempHistoryAtom,
  rightSensorTempHistoryAtom,
  leftStrokeLengthHistoryAtom,
  rightStrokeLengthHistoryAtom,
  leftCardiacOutputHistoryAtom,
  rightCardiacOutputHistoryAtom,
  leftPowerConsumptionHistoryAtom,
  rightPowerConsumptionHistoryAtom
} from '@/lib/datastore.ts';
import { HistoryData } from '@/types/history.ts';
import InternalPressureSensors from './InternalPressureSensors';
interface HeartMetrics {
  PowerConsumption: PressureSensor;
  IntPressure: PressureSensor;
  MedicalPressure: PressureSensor;
  IntPressureMin: number;
  IntPressureMax: number;
  CardiacOutput: PressureSensor;
  ActualStrokeLen: string;
  TargetStrokeLen: string;
  SensorTemperature: string;
  ThermistorTemperature: string;
  CpuLoad: string;
  OutflowPressure: string;
}

interface MetricsGridProps {
  leftMetrics?: HeartMetrics;
  rightMetrics?: HeartMetrics;
}

interface DataRateGaugeRef {
  addMessage: (message: any) => void;
}

interface WebView2WebMessageReceivedEventArgs {
  data: any;
}

const defaultMetrics: HeartMetrics = {
  //StrokeVolume: "0",
  PowerConsumption: { MeanValue: "0", MinValue: "0", MaxValue: "0", BackColor: "" },
  IntPressure: { MeanValue: "0", MinValue: "0", MaxValue: "0", BackColor: "" },
  MedicalPressure: { MeanValue: "0", MinValue: "0", MaxValue: "0", BackColor: "" },
  IntPressureMin: 0,
  IntPressureMax: 0,
  CardiacOutput: { MeanValue: "0", MinValue: "0", MaxValue: "0", BackColor: "" },
  ActualStrokeLen: "0",
  TargetStrokeLen: "0",
  SensorTemperature: "0",
  ThermistorTemperature: "0",
  CpuLoad: "0",
  OutflowPressure: "0"
};

// Memoized history atom selector
const createHistoryAtoms = (isLeftHeart: boolean) => ({
  outflowHistory: isLeftHeart ? leftOutflowPressureHistoryAtom : rightOutflowPressureHistoryAtom,
  thermistorHistory: isLeftHeart ? leftThermistorTempHistoryAtom : rightThermistorTempHistoryAtom,
  sensorHistory: isLeftHeart ? leftSensorTempHistoryAtom : rightSensorTempHistoryAtom,
  strokeLengthHistory: isLeftHeart ? leftStrokeLengthHistoryAtom : rightStrokeLengthHistoryAtom,
  cardiacOutputHistory: isLeftHeart ? leftCardiacOutputHistoryAtom : rightCardiacOutputHistoryAtom,
  powerConsumptionHistory: isLeftHeart ? leftPowerConsumptionHistoryAtom : rightPowerConsumptionHistoryAtom
});

// Create a computed signal for pressure sensor data
const createPressureSensorSignal = (isLeftHeart: boolean) => {
  return computed(() => {
    const heartData = isLeftHeart ? leftHeartSignal.value : rightHeartSignal.value;
    if (!heartData?.IntPressure) return null;
    
    return {
      ...heartData.IntPressure,
      MinValue: heartData.IntPressureMin?.toString() ?? "0",
      MaxValue: heartData.IntPressureMax?.toString() ?? "0"
    };
  });
};

// Memoized update history function
const updateHistory = (
  prev: HistoryData | null,
  value: number | string,
  now: number,
  MAX_POINTS: number
): HistoryData => ({
  times: [...(prev?.times || []), now].slice(-MAX_POINTS),
  oscillations: [...(prev?.oscillations || []), parseFloat(value?.toString() || "0")].slice(-MAX_POINTS),
  mean: null
});

// Memoized metric cards
const TopRowMetrics = React.memo(({ metrics, historyAtoms, isLeftHeart }: { 
  metrics: HeartMetrics; 
  historyAtoms: ReturnType<typeof createHistoryAtoms>;
  isLeftHeart: boolean;
}) => (
  <div className={`grid grid-cols-1 md:grid-cols-3 gap-4 mb-4 w-auto min-w-0 auto-rows-fr ${isLeftHeart ? 'justify-items-end' : 'justify-items-start'}`}>
    {isLeftHeart ? (
      <>
        <MetricCard
          title="Outflow Pressure (Estimated)"
          value={parseFloat(metrics.OutflowPressure ?? "0")}
          unit="mmHg"
          status="error"
          showArrows
          arrowValue={-1}
          historyAtom={historyAtoms.outflowHistory}
          baselineValue={15}
          redHi={true}
        />
        <MetricCard
          title="Stroke Length"
          value={parseFloat(metrics.ActualStrokeLen ?? "0")}
          unit="mm"
          showArrows
          arrowValue={15}
          historyAtom={historyAtoms.strokeLengthHistory}
          baselineValue={15}
          redHi={false}
        />
        <HiLoPressureCard
          title="Left Atrium Pressure (Internal)"
          unit="mmHg"
          sensorSignal={createPressureSensorSignal(true)}
          showArrows
          arrowValue={15}
        />
      </>
    ) : (
      <>
        <HiLoPressureCard
          title="Right Atrium Pressure (Internal)"
          unit="mmHg"
          sensorSignal={createPressureSensorSignal(false)}
          showArrows
          arrowValue={15}
        />
        <MetricCard
          title="Stroke Length"
          value={parseFloat(metrics.ActualStrokeLen ?? "0")}
          unit="mm"
          showArrows
          arrowValue={15}
          historyAtom={historyAtoms.strokeLengthHistory}
          baselineValue={15}
          redHi={false}
        />
        <MetricCard
          title="Outflow Pressure (Estimated)"
          value={parseFloat(metrics.OutflowPressure ?? "0")}
          unit="mmHg"
          status="error"
          showArrows
          arrowValue={-1}
          historyAtom={historyAtoms.outflowHistory}
          baselineValue={15}
          redHi={true}
        />
      </>
    )}
  </div>
));
TopRowMetrics.displayName = 'TopRowMetrics';

const BottomRowMetrics = React.memo(({ metrics, historyAtoms }: { 
  metrics: HeartMetrics; 
  historyAtoms: ReturnType<typeof createHistoryAtoms>;
}) => (
  <div className="grid grid-cols-1 md:grid-cols-3 gap-4 w-auto min-w-0 auto-rows-fr justify-items-end">
    <DualMetricCard
      topMetric={{
        title: "Thermistor Temperature",
        value: parseFloat(metrics.ThermistorTemperature ?? "0"),
        unit: "°C",
        status: "warning",
        historyAtom: historyAtoms.thermistorHistory,
      }}
      bottomMetric={{
        title: "Sensor Temperature",
        value: parseFloat(metrics.SensorTemperature ?? "0"),
        unit: "°C",
        historyAtom: historyAtoms.sensorHistory,
      }}
      baselineValue={30}
      redHi={true}
    />
    <MetricCard 
      title="Average Power" 
      value={parseFloat(metrics.PowerConsumption?.MeanValue ?? "0")}
      unit="W"
      showArrows
      arrowValue={15}
      historyAtom={historyAtoms.powerConsumptionHistory}
      baselineValue={8}
      redHi={true}
    />
    <MetricCard
      title="Cardiac Output (Theoretical)"
      value={parseFloat(metrics.CardiacOutput?.MeanValue ?? "0")}
      unit="l/min"
      showArrows
      arrowValue={-1}
      historyAtom={historyAtoms.cardiacOutputHistory}
      baselineValue={3}
      redHi={false}
    />
  </div>
));
BottomRowMetrics.displayName = 'BottomRowMetrics';

const RightHeartBottomRowMetrics = React.memo(({ metrics, historyAtoms }: { 
  metrics: HeartMetrics; 
  historyAtoms: ReturnType<typeof createHistoryAtoms>;
}) => (
  <div className="grid grid-cols-1 md:grid-cols-3 gap-4 w-auto min-w-0 auto-rows-fr justify-items-start">
    <MetricCard
      title="Cardiac Output (Theoretical)"
      value={parseFloat(metrics.CardiacOutput?.MeanValue ?? "0")}
      unit="l/min"
      showArrows
      arrowValue={-1}
      historyAtom={historyAtoms.cardiacOutputHistory}
      baselineValue={5}
      redHi={false}
    />
    <MetricCard 
      title="Average Power" 
      value={parseFloat(metrics.PowerConsumption?.MeanValue ?? "0")}
      unit="W"
      showArrows
      arrowValue={15}
      historyAtom={historyAtoms.powerConsumptionHistory}
      baselineValue={4}
      redHi={true}
    />
    <DualMetricCard
      topMetric={{
        title: "Thermistor Temperature",
        value: parseFloat(metrics.ThermistorTemperature ?? "0"),
        unit: "°C",
        status: "warning",
        historyAtom: historyAtoms.thermistorHistory,
      }}
      bottomMetric={{
        title: "Sensor Temperature",
        value: parseFloat(metrics.SensorTemperature ?? "0"),
        unit: "°C",
        historyAtom: historyAtoms.sensorHistory,
      }}
      baselineValue={30}
      redHi={true}
    />
  </div>
));
RightHeartBottomRowMetrics.displayName = 'RightHeartBottomRowMetrics';

export const HeartSection = React.memo(({
  metrics = defaultMetrics,
  title,
}: {
  metrics?: HeartMetrics;
  title: string;
}) => {
  const isLeftHeart = title === "LEFT HEART";
  
  // Memoize history atoms
  const historyAtoms = useMemo(() => createHistoryAtoms(isLeftHeart), [isLeftHeart]);
  
  // Get all setters at once
  const setOutflowHistory = useSetAtom(historyAtoms.outflowHistory);
  const setThermistorHistory = useSetAtom(historyAtoms.thermistorHistory);
  const setSensorHistory = useSetAtom(historyAtoms.sensorHistory);
  //const setStrokeVolumeHistory = useSetAtom(historyAtoms.strokeVolumeHistory);
  const setStrokeLengthHistory = useSetAtom(historyAtoms.strokeLengthHistory);
  const setCardiacOutputHistory = useSetAtom(historyAtoms.cardiacOutputHistory);
  const setPowerConsumptionHistory = useSetAtom(historyAtoms.powerConsumptionHistory);

  // Update history when metrics change
  useEffect(() => {
    const now = Date.now();
    const MAX_POINTS = 500; // ~25 minutes of data at 0.33Hz
    
    //setStrokeVolumeHistory(prev => updateHistory(prev, metrics.StrokeVolume, now, MAX_POINTS));
    setStrokeLengthHistory(prev => updateHistory(prev, metrics.ActualStrokeLen, now, MAX_POINTS));
    setCardiacOutputHistory(prev => updateHistory(prev, metrics.CardiacOutput?.MeanValue || "0", now, MAX_POINTS));
    setPowerConsumptionHistory(prev => updateHistory(prev, metrics.PowerConsumption?.MeanValue || "0", now, MAX_POINTS));
    setThermistorHistory(prev => updateHistory(prev, metrics.ThermistorTemperature, now, MAX_POINTS));
    setSensorHistory(prev => updateHistory(prev, metrics.SensorTemperature, now, MAX_POINTS));
    setOutflowHistory(prev => updateHistory(prev, metrics.OutflowPressure, now, MAX_POINTS));
  }, [
    metrics,
    //set6umeHistory,
    setStrokeLengthHistory,
    setCardiacOutputHistory,
    setPowerConsumptionHistory,
    setThermistorHistory,
    setSensorHistory,
    setOutflowHistory
  ]);

  return (
    <div className="border border-neutral-700 rounded-lg p-2 sm:px-4">
      <div className="text-xl px-2 font-bold text-white mb-4 text-center my-4">{title}</div>
      <div className={`flex flex-col gap-4 ${isLeftHeart ? 'items-end' : 'items-start'}`}>
        <TopRowMetrics metrics={metrics} historyAtoms={historyAtoms} isLeftHeart={isLeftHeart} />
        {isLeftHeart ? (
          <BottomRowMetrics metrics={metrics} historyAtoms={historyAtoms} />
        ) : (
          <RightHeartBottomRowMetrics metrics={metrics} historyAtoms={historyAtoms} />
        )}
      </div>
    </div>
  );
}, (prevProps, nextProps) => {
  return (
    prevProps.metrics === nextProps.metrics &&
    prevProps.title === nextProps.title
  );
});

HeartSection.displayName = 'HeartSection';

const ControlSection = React.memo(({ isChecked, setIsChecked }: { isChecked: boolean; setIsChecked: (checked: boolean) => void }) => (
  <div className="flex flex-col items-center gap-4">
    <OperationModeToggle
      isChecked={isChecked}
      onToggle={setIsChecked}
      switchId="manual-or-auto-mode"
      mode={isChecked ? 'manual' : 'auto'}
    />
    <HeartControl />
  </div>
));
ControlSection.displayName = 'ControlSection';

const ChartSection = React.memo(({ showingLeftHeart, setShowingLeftHeart }: { 
  showingLeftHeart: boolean; 
  setShowingLeftHeart: (show: boolean) => void 
}) => (
  <div className="col-span-2 bg-neutral-900 rounded-lg mb-6 h-[300px] overflow-hidden shadow-[inset_0_0_0_1px_rgb(82,82,91)]">
    <div className="w-full h-full p-2">
      <PressureChart showingLeftHeart={showingLeftHeart} />
      <button
        onClick={() => setShowingLeftHeart(!showingLeftHeart)}
        className="px-2 py-1 bg-neutral-900 text-gray-400 text-sm rounded hover:bg-neutral-800 transition-colors"
      >
        {showingLeftHeart ? 'Show Right Heart' : 'Show Left Heart'}
      </button>
    </div>
  </div>
));
ChartSection.displayName = 'ChartSection';

const PhysiologicalControlSection = React.memo(({ isChecked }: { isChecked: boolean }) => (
  <div className="col-span-2 bg-neutral-900 rounded-lg mb-6 px-6 py-0 h-[300px] flex flex-col items-center justify-center text-gray-400 shadow-[inset_0_0_0_1px_rgb(82,82,91)]">
    <PhysiologicalControl disabled={!isChecked} />
  </div>
));
PhysiologicalControlSection.displayName = 'PhysiologicalControlSection';

const PressureSensorSection = React.memo(() => (
  <div className="col-span-1 bg-neutral-900 rounded-lg mb-6 p-6 h-[300px] flex flex-col items-center justify-center text-gray-400 shadow-[inset_0_0_0_1px_rgb(82,82,91)]">
    <div className="flex flex-col items-center gap-4">
      <InternalPressureSensors />
    </div>
  </div>
));
PressureSensorSection.displayName = 'PressureSensorSection';

const MetricsGrid: React.FC<MetricsGridProps> = React.memo(({ leftMetrics = defaultMetrics, rightMetrics = defaultMetrics }) => {
  // Make component reactive to signals
  useSignals();
  
  // Access the signal values directly
  const leftHeart = leftHeartSignal.value || defaultMetrics;
  const rightHeart = rightHeartSignal.value || defaultMetrics;
  
  const [isChecked, setIsChecked] = React.useState(false);  
  const dataRateGaugeRef = useRef<DataRateGaugeRef>(null);
  const [showingLeftHeart, setShowingLeftHeart] = React.useState(true);
  
    return (
    <div className="space-y-8 mt-8">
      <div className="mx-4 mt-8">
        <div className="flex flex-col md:flex-row items-center justify-center rounded-lg p-2 sm:p-4 shadow-[inset_0_0_0_1px_rgb(82,82,91)] gap-4 overflow-hidden">
          <div className="w-full md:w-5/12 flex justify-end">
            <HeartSection metrics={leftHeart} title="LEFT HEART" />
          </div>
          <div className="w-full md:w-2/12">
            <ControlSection isChecked={isChecked} setIsChecked={setIsChecked} />
          </div>
          <div className="w-full md:w-5/12 flex justify-start">
            <HeartSection metrics={rightHeart} title="RIGHT HEART" />
          </div>
        </div>
      </div>

      <div className="grid grid-cols-5 gap-8 pb-4w-full mx-4">
        <PhysiologicalControlSection isChecked={isChecked} />
        <PressureSensorSection />
        <ChartSection showingLeftHeart={showingLeftHeart} setShowingLeftHeart={setShowingLeftHeart} />
      </div>
    </div>
  );
}, (prevProps, nextProps) => {
  return (
    prevProps.leftMetrics === nextProps.leftMetrics &&
    prevProps.rightMetrics === nextProps.rightMetrics
  );
});

MetricsGrid.displayName = 'MetricsGrid';

export default MetricsGrid;
