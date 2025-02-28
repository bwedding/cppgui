import React, { useState } from 'react';

interface InternalPressureSensorsProps {
  // Add any props if needed
}

const InternalPressureSensors: React.FC<InternalPressureSensorsProps> = () => {
  const [leftAtrialPressure, setLeftAtrialPressure] = useState(0);
  const [rightAtrialPressure, setRightAtrialPressure] = useState(0);

  const handleIncrement = (setter: React.Dispatch<React.SetStateAction<number>>, value: number) => {
    setter(value + 1);
  };

  const handleDecrement = (setter: React.Dispatch<React.SetStateAction<number>>, value: number) => {
    setter(value - 1);
  };

  return (
    <div className="bg-neutral-900 text-gray-100 mt-0 p-3 rounded-lg shadow-lg w-full">
      <h2 className="text-lg font-bold mb-4 mt-0 text-white text-center">Internal Pressure Sensors</h2>
      
      <div className="flex flex-col gap-4">
        {/* Left Atrial Pressure */}
        <div className="flex flex-col">
          <label className="text-xs font-medium text-gray-300 mb-1">
            LAP
          </label>
          <div className="flex gap-2">
            <div className="flex">
              <input
                type="number"
                name="leftAtrialPressure"
                id="leftAtrialPressure"
                value={leftAtrialPressure}
                onChange={(e) => setLeftAtrialPressure(parseInt(e.target.value) || 0)}
                className="bg-neutral-900 border border-neutral-700 text-white py-0 px-2 rounded-l w-16 text-center focus:outline-none [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none"
              />
              <div className="flex flex-col">
                <button
                  onClick={() => handleIncrement(setLeftAtrialPressure, leftAtrialPressure)}
                  className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-tr border-t border-r border-neutral-600 text-xs"
                >
                  ▲
                </button>
                <button
                  onClick={() => handleDecrement(setLeftAtrialPressure, leftAtrialPressure)}
                  className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-tr border-t border-r border-neutral-600 text-xs"
                >
                  ▼
                </button>
              </div>
            </div>
            <button
              className="bg-neutral-600 hover:bg-neutral-500 text-white px-2 py-1 rounded text-sm"
            >
              Cal Left
            </button>
          </div>
        </div>

        {/* Right Atrial Pressure */}
        <div className="flex flex-col">
          <label className="text-xs font-medium text-gray-300 mb-1">
            RAP
          </label>
          <div className="flex gap-2">
            <div className="flex">
              <input
                type="number"
                value={rightAtrialPressure}
                onChange={(e) => setRightAtrialPressure(parseInt(e.target.value) || 0)}
                className="bg-neutral-900 border border-neutral-700 text-white py-0 px-2 rounded-l w-16 text-center focus:outline-none [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none"
              />
              <div className="flex flex-col">
                <button
                  onClick={() => handleIncrement(setRightAtrialPressure, rightAtrialPressure)}
                  className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-tr border-t border-r border-neutral-600 text-xs"
                >
                  ▲
                </button>
                <button
                  onClick={() => handleDecrement(setRightAtrialPressure, rightAtrialPressure)}
                  className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-tr border-t border-r border-neutral-600 text-xs"
                >
                  ▼
                </button>
              </div>
            </div>
            <button
              className="bg-neutral-600 hover:bg-neutral-500 text-white px-2 py-1 rounded text-sm"
            >
              Cal Right
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};

export default InternalPressureSensors;
