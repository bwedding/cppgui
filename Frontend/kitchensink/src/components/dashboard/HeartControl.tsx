import { memo, useEffect, useMemo } from "react";
import { Button } from "@/components/ui/button.tsx";
import { Minus, Plus } from "lucide-react";
import heartImage from "/public/images/heart.png";
import { heartMetricsAtom } from '@/lib/datastore.ts';
import { useAtomValue } from 'jotai';

interface HeartControlProps {
  minBpm?: number;
  maxBpm?: number;
  onBpmChange?: (bpm: number) => void;
}

// Memoized header components
const Header = memo(() => (
  <>
    <div className="text-gray-400 text-xl font-bold text-center">
      HEART RATE
    </div>
    <div className="text-gray-400 text-lg font-medium text-center mb-2">
      (bpm)
    </div>
  </>
));

// Memoized heart image component
const HeartImage = memo(() => (
  <img
    src={heartImage}
    alt="Beating Heart"
    className="w-[180px] h-[180px] object-contain animate-pulse"
  />
));

// Memoized control button component
const ControlButton = memo(({ 
  action, 
  disabled, 
  bpm 
}: { 
  action: 'increase' | 'decrease';
  disabled: boolean;
  bpm: number;
}) => (
  <Button
    buttonId="heart-rate-control"
    eventMetadata={{
      action,
      control: 'heart-rate',
      parameter: 'bpm',
      currentState: bpm
    }}
    variant="ghost"
    size="icon"
    disabled={disabled}
    className="bg-neutral-800 text-gray-400 hover:text-white hover:bg-neutral-600 active:bg-neutral-700 active:translate-y-[2px]"
  >
    {action === 'decrease' ? <Minus className="h-6 w-6" /> : <Plus className="h-6 w-6" />}
  </Button>
));

// Memoized BPM display component
const BpmDisplay = memo(({ bpm }: { bpm: number }) => (
  <div className="text-4xl font-bold text-white min-w-[80px] text-center">
    {bpm}
  </div>
));

// Memoized controls section component
const ControlsSection = memo(({ 
  bpm, 
  minBpm, 
  maxBpm 
}: { 
  bpm: number;
  minBpm: number;
  maxBpm: number;
}) => (
  <div className="flex items-center justify-between gap-4">
    <ControlButton action="decrease" disabled={bpm <= minBpm} bpm={bpm} />
    <BpmDisplay bpm={bpm} />
    <ControlButton action="increase" disabled={bpm >= maxBpm} bpm={bpm} />
  </div>
));

const HeartControl = memo(({
  minBpm = 15,
  maxBpm = 155,
  onBpmChange = () => {},
}: HeartControlProps) => {
  const data = useAtomValue(heartMetricsAtom);
  const bpm = Number(data?.heartRate) || 85;

  // Call onBpmChange when bpm changes
  useEffect(() => {
    onBpmChange(bpm);
  }, [bpm, onBpmChange]);

  return (
    <div className="w-[220px] h-[320px]m-4 bg-neutral-900 rounded-lg p-4 space-y-4 mt-4">
      <div className="relative w-full aspect-square flex items-center justify-center" style={{ animationDuration: `${120 / bpm}s` }}>
        <HeartImage />
      </div>
      <Header />
      <ControlsSection bpm={bpm} minBpm={minBpm} maxBpm={maxBpm} />
    </div>
  );
}, (prevProps, nextProps) => {
  // Custom comparison function to prevent unnecessary re-renders
  return (
    prevProps.minBpm === nextProps.minBpm &&
    prevProps.maxBpm === nextProps.maxBpm &&
    prevProps.onBpmChange === nextProps.onBpmChange
  );
});

// Add display names for better debugging
Header.displayName = 'Header';
HeartImage.displayName = 'HeartImage';
ControlButton.displayName = 'ControlButton';
BpmDisplay.displayName = 'BpmDisplay';
ControlsSection.displayName = 'ControlsSection';
HeartControl.displayName = 'HeartControl';

export default HeartControl;
