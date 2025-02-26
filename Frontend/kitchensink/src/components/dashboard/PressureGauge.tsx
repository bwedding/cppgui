import React, { useState, useEffect, useRef } from 'react';
import { Atom, useAtomValue } from 'jotai';

interface PressureData {
  primary: number;
  high: number;
  low: number;
}

interface PressureGaugeProps {
  pressureAtom: Atom<PressureData | null>;
}

const PressureGauge: React.FC<PressureGaugeProps> = ({ pressureAtom }) => {
  const [currentValue, setCurrentValue] = useState(0);
  const [highValue, setHighValue] = useState(0);
  const [lowValue, setLowValue] = useState(0);
  const lastUpdateRef = useRef(Date.now());
  const smoothedValueRef = useRef(0);
  const smoothedHighRef = useRef(0);
  const smoothedLowRef = useRef(0);
  const pressure = useAtomValue(pressureAtom);
  
  const RANGE = { MIN: -40, MAX: 40 };
  const DAMPING_FACTOR = 0.94;

  // Debug logging

  useEffect(() => {
    if (pressure) {
      smoothedValueRef.current = pressure.primary;
      smoothedHighRef.current = pressure.high;
      smoothedLowRef.current = pressure.low;
    } else {
      console.warn('No pressure data available');
      smoothedValueRef.current = 0;
      smoothedHighRef.current = 0;
      smoothedLowRef.current = 0;
    }
  }, [pressure]);

  useEffect(() => {
    const interval = setInterval(() => {
      const now = Date.now();
      const elapsed = (now - lastUpdateRef.current) / 1000;
      
      if (elapsed > 0) {
        // Apply exponential smoothing to all values
        smoothedValueRef.current = (DAMPING_FACTOR * smoothedValueRef.current) + 
                                 ((1 - DAMPING_FACTOR) * (pressure ? pressure.primary : 0));
        smoothedHighRef.current = (DAMPING_FACTOR * smoothedHighRef.current) + 
                                ((1 - DAMPING_FACTOR) * (pressure ? pressure.high : 0));
        smoothedLowRef.current = (DAMPING_FACTOR * smoothedLowRef.current) + 
                               ((1 - DAMPING_FACTOR) * (pressure ? pressure.low : 0));
        
        setCurrentValue(smoothedValueRef.current);
        setHighValue(smoothedHighRef.current);
        setLowValue(smoothedLowRef.current);
        lastUpdateRef.current = now;
      }
    }, 50);

    return () => clearInterval(interval);
  }, [pressure]);

  const valueToRotation = (value: number): number => {
    const clampedValue = Math.max(RANGE.MIN, Math.min(RANGE.MAX, value));
    return ((clampedValue - RANGE.MIN) / (RANGE.MAX - RANGE.MIN)) * 270 - 135;
  };

  const ticks = [-40, -30, -20, -10, 0, 10, 20, 30, 40].map((value) => ({
    rotation: valueToRotation(value),
    label: value.toString()
  }));

  return (
    <div className="w-64 h-64 relative flex flex-col items-center">
      {/* Dark background */}
      <div className="absolute inset-0 bg-neutral-900 rounded-full">
        <div className="absolute inset-4 bg-neutral-800 rounded-full"></div>
      </div>
      
      {/* Tick marks */}
      {ticks.map(({ rotation, label }) => (
        <React.Fragment key={label}>
          <div
            className="absolute w-0.5 h-2 bg-gray-500 rounded transform origin-bottom"
            style={{
              left: 'calc(50% - 1px)',
              bottom: '50%',
              transform: `rotate(${rotation}deg) translateY(-116px)`
            }}
          />
          <div
            className="absolute text-xs text-gray-400 transform -translate-x-1/2"
            style={{
              left: '50%',
              bottom: '50%',
              transform: `rotate(${rotation}deg) translateY(-90px) rotate(${-rotation}deg)`
            }}
          >
            {label}
          </div>
        </React.Fragment>
      ))}
      
      {/* High and Low Needles (smaller) */}
      <div 
        className="absolute w-0.5 h-24 bg-yellow-500 rounded origin-bottom transform transition-transform duration-150 ease-out opacity-50"
        style={{
          left: 'calc(50% - 1px)',
          bottom: '50%',
          transform: `rotate(${valueToRotation(highValue)}deg)`
        }}
      >
        <div className="absolute top-0 left-1/2 w-1.5 h-1.5 bg-yellow-500 rounded-full transform -translate-x-1/2" />
      </div>
      
      <div 
        className="absolute w-0.5 h-24 bg-blue-500 rounded origin-bottom transform transition-transform duration-150 ease-out opacity-50"
        style={{
          left: 'calc(50% - 1px)',
          bottom: '50%',
          transform: `rotate(${valueToRotation(lowValue)}deg)`
        }}
      >
        <div className="absolute top-0 left-1/2 w-1.5 h-1.5 bg-blue-500 rounded-full transform -translate-x-1/2" />
      </div>
      
      {/* Primary Needle */}
      <div 
        className="absolute w-0.5 h-32 bg-red-500 rounded origin-bottom transform transition-transform duration-150 ease-out"
        style={{
          left: 'calc(50% - 1px)',
          bottom: '50%',
          transform: `rotate(${valueToRotation(currentValue)}deg)`
        }}
      >
        <div className="absolute top-0 left-1/2 w-2 h-2 bg-red-500 rounded-full transform -translate-x-1/2" />
      </div>
      
      {/* Value Display */}
      <div className="absolute inset-0 flex items-center justify-center">
        <div className="text-white text-2xl font-bold">
          {isNaN(currentValue) ? 'NaN' : currentValue.toFixed(0)}
        </div>
        <div className="absolute mt-8 text-gray-400 text-sm">mmHg</div>
      </div>
      
      {/* Title */}
      <div className="absolute -bottom-4 text-gray-400 text-lg">
        Left Atrium Pressure
      </div>
    </div>
  );
};

export default PressureGauge;