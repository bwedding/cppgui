import { useEffect, useMemo, memo, useState } from "react";
import { useSignals } from "@preact/signals-react/runtime";
import { Button } from "@/components/ui/button.tsx";
import { Minus, Plus } from "lucide-react";
import heartImage from "/public/images/heart.png";
import { heartRateSignal } from '@/lib/datastore.ts';
import { computed, Signal } from "@preact/signals-react";
import { useEventStore } from "@/lib/EventStore.ts";
import { ButtonClickEvent } from "@/types/types.ts";

interface HeartControlProps {
  minBpm?: number;
  maxBpm?: number;
  onBpmChange?: (bpm: number) => void;
}

// Header components - static, never needs to re-render
const Header = () => (
  <>
    <div className="text-gray-400 text-xl font-bold text-center">
      HEART RATE
    </div>
    <div className="text-gray-400 text-lg font-medium text-center mb-2">
      (bpm)
    </div>
  </>
);

// Heart image component - only re-renders when animation duration changes
const HeartImage = ({ bpm }: { bpm: number }) => {
  const animationDuration = `${120 / bpm}s`;
  
  return (
    <div className="relative w-full aspect-square flex items-center justify-center" style={{ animationDuration }}>
      <img
        src={heartImage}
        alt="Beating Heart"
        className="w-[180px] h-[180px] object-contain animate-pulse"
      />
    </div>
  );
};

// Static button component that doesn't depend on signals directly
const ControlButton = memo(({ 
  action, 
  disabled, 
  bpm,
  onClick
}: { 
  action: 'increase' | 'decrease';
  disabled: boolean;
  bpm: number;
  onClick?: () => void;
}) => {
  const handleClick = (e: React.MouseEvent<HTMLButtonElement>) => {
    console.log(`${action} button clicked, disabled: ${disabled}, bpm: ${bpm}`);
    if (onClick) {
      onClick();
    }
  };

  return (
    <Button
      name = {`change-heartrate-${action}`}
      variant="ghost"
      size="icon"
      disabled={disabled}
      onClick={handleClick}
      data-backend='{"control":"change-heartrate","parameter":"bpm","currentState":"decrease or increase depending on action"}'
      className="bg-neutral-800 text-gray-400 hover:text-white hover:bg-neutral-600 active:bg-neutral-700 active:translate-y-[2px]"
      noEvent={true}
    >
      {action === 'decrease' ? <Minus className="h-6 w-6" /> : <Plus className="h-6 w-6" />}
    </Button>
  );
});

// Main component - stable container that doesn't re-render
const HeartControl = ({
  minBpm = 15,
  maxBpm = 155,
  onBpmChange = () => {},
}: HeartControlProps) => {
  const [localBpm, setLocalBpm] = useState(Number(heartRateSignal.value) || 85);
  
  // Update the local state when the signal changes
  useEffect(() => {
    const currentBpm = Number(heartRateSignal.value) || 85;
    setLocalBpm(currentBpm);
  }, [heartRateSignal.value]);
  
  // Create computed signals for the disabled states based on local state
  const decreaseDisabled = useMemo(() => localBpm <= minBpm, [localBpm, minBpm]);
  const increaseDisabled = useMemo(() => localBpm >= maxBpm, [localBpm, maxBpm]);
  
  // For debugging - check if useEventStore.getState() is working
  useEffect(() => {
    const eventStore = useEventStore.getState();
    console.log('EventStore state available:', !!eventStore, 'sendEvent available:', !!eventStore.sendEvent);
  }, []);
  
  const handleDecrease = () => {
    console.log('Decrease button clicked, disabled:', decreaseDisabled);
    if (!decreaseDisabled) {
      const newBpm = localBpm - 1;
      console.log('Setting new BPM:', newBpm);
      setLocalBpm(newBpm);
      onBpmChange(newBpm);
      
      const sendEvent = useEventStore.getState().sendEvent;
      const event: ButtonClickEvent = {
        type: 'BUTTON_CLICK',
        buttonId: 'decrease-heartrate',
        value: true,
        source: 'user-interface',
        timestamp: Date.now(),
        action: 'decrease',
        control: 'heart-rate',
        parameter: 'bpm',
        currentState: newBpm,
        metadata: {
          action: 'decrease',
          control: 'heart-rate',
          parameter: 'bpm',
          currentState: newBpm
        }
      };
      
      console.log('Sending decrease button click event', event);
      sendEvent(event);
    }
  };
  
  const handleIncrease = () => {
    console.log('Increase button clicked, disabled:', increaseDisabled);
    if (!increaseDisabled) {
      const newBpm = localBpm + 1;
      console.log('Setting new BPM:', newBpm);
      setLocalBpm(newBpm);
      onBpmChange(newBpm);
      
      const sendEvent = useEventStore.getState().sendEvent;
      const event: ButtonClickEvent = {
        type: 'BUTTON_CLICK',
        buttonId: 'increase-heartrate',
        value: true,
        source: 'user-interface',
        timestamp: Date.now(),
        action: 'increase',
        control: 'heart-rate',
        parameter: 'bpm',
        currentState: newBpm,
        metadata: {
          action: 'increase',
          control: 'heart-rate',
          parameter: 'bpm',
          currentState: newBpm
        }
      };
      
      console.log('Sending increase button click event', event);
      sendEvent(event);
    }
  };
  
  return (
    <div className="w-[220px] h-[320px]m-4 bg-neutral-900 rounded-lg p-4 space-y-4 mt-4">
      <HeartImage bpm={localBpm} />
      <Header />
      <div className="flex items-center justify-between gap-4">
        <ControlButton 
          action="decrease" 
          disabled={decreaseDisabled} 
          bpm={localBpm} 
          onClick={handleDecrease} 
        />
        <div className="text-4xl font-bold text-white min-w-[80px] text-center">
          {localBpm}
        </div>
        <ControlButton 
          action="increase" 
          disabled={increaseDisabled} 
          bpm={localBpm} 
          onClick={handleIncrease} 
        />
      </div>
    </div>
  );
};

// Add display names for better debugging
Header.displayName = 'Header';
HeartImage.displayName = 'HeartImage';
ControlButton.displayName = 'ControlButton';
HeartControl.displayName = 'HeartControl';

export default HeartControl;
