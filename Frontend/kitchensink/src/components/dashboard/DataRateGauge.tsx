import React, { useState, useEffect, useRef, forwardRef, useImperativeHandle } from 'react';

const DualDataRateGauge = forwardRef((props, ref) => {
  const [charRate, setCharRate] = useState(0);
  const [packetRate, setPacketRate] = useState(0);
  const charactersRef = useRef(0);
  const packetsRef = useRef(0);
  const lastUpdateRef = useRef(Date.now());
  const smoothedCharRateRef = useRef(0);
  const smoothedPacketRateRef = useRef(0);
  const MAX_CHAR_RATE = 100000;
  const MAX_PACKET_RATE = 60;
  const DAMPING_FACTOR = 0.94;

  useImperativeHandle(ref, () => ({
    addMessage: (message) => {
      try {
        const chars = typeof message === 'string' ? 
          message.length : 
          JSON.stringify(message).length;
        charactersRef.current += chars;
        packetsRef.current += 1;
      } catch (error) {
        console.error('Error processing message:', error);
      }
    }
  }));

  useEffect(() => {
    const interval = setInterval(() => {
      const now = Date.now();
      const elapsed = (now - lastUpdateRef.current) / 1000;
      
      if (elapsed > 0) {
        const instantCharRate = charactersRef.current / elapsed;
        const instantPacketRate = packetsRef.current / elapsed;
        
        // Apply exponential smoothing to both rates
        smoothedCharRateRef.current = (DAMPING_FACTOR * smoothedCharRateRef.current) + 
                                    ((1 - DAMPING_FACTOR) * instantCharRate);
        smoothedPacketRateRef.current = (DAMPING_FACTOR * smoothedPacketRateRef.current) + 
                                      ((1 - DAMPING_FACTOR) * instantPacketRate);
        
        setCharRate(smoothedCharRateRef.current);
        setPacketRate(smoothedPacketRateRef.current);
        charactersRef.current = 0;
        packetsRef.current = 0;
        lastUpdateRef.current = now;
      }
    }, 50);

    return () => clearInterval(interval);
  }, []);

  const Gauge = ({ rate, maxRate, formatValue, ticks, title, unit }) => {
    const rotation = Math.min(rate, maxRate) * (270 / maxRate) - 135;
    
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
        
        {/* Needle */}
        <div 
          className="absolute w-0.5 h-32 bg-red-500 rounded origin-bottom transform transition-transform duration-150 ease-out"
          style={{
            left: 'calc(50% - 1px)',
            bottom: '50%',
            transform: `rotate(${rotation}deg)`
          }}
        >
          <div className="absolute top-0 left-1/2 w-2 h-2 bg-red-500 rounded-full transform -translate-x-1/2" />
        </div>
        
        {/* Central value display */}
        <div className="absolute inset-0 flex flex-col items-center justify-center">
          <div className="text-3xl font-bold text-gray-200 font-mono tracking-wider">
            {formatValue(rate)}
          </div>
          <div className="text-sm text-gray-400 mt-1">{unit}</div>
        </div>
        
        {/* Title */}
        <div className="absolute -bottom-4 text-gray-400 text-lg">
          {title}
        </div>
      </div>
    );
  };

  const charTicks = [0, 20, 40, 60, 80, 100].map((value) => ({
    rotation: (value * 1000 * (270 / MAX_CHAR_RATE)) - 135,
    label: `${value}k`
  }));

  const packetTicks = [0, 10, 20, 30, 40, 50, 60].map((value) => ({
    rotation: (value * (270 / MAX_PACKET_RATE)) - 135,
    label: value.toString()
  }));

  return (
    <div className="flex gap-8 items-center">
      <Gauge
        rate={charRate}
        maxRate={MAX_CHAR_RATE}
        formatValue={(rate) => `${(rate/1000).toFixed(1)}k`}
        ticks={charTicks}
        title="Character Rate"
        unit="chars/sec"
      />
      <Gauge
        rate={packetRate}
        maxRate={MAX_PACKET_RATE}
        formatValue={(rate) => rate.toFixed(1)}
        ticks={packetTicks}
        title="Packet Rate"
        unit="packets/sec"
      />
    </div>
  );
});

export default DualDataRateGauge;