import React, { useState, useMemo } from 'react';
import { Accordion, AccordionContent, AccordionItem, AccordionTrigger } from "@/components/ui/accordion";

interface PhysiologicalControlProps {
  disabled?: boolean;
}

const PhysiologicalControl = ({ disabled = false }: PhysiologicalControlProps) => {
      const [leftAtrialPressure, setLeftAtrialPressure] = useState(10);
      const [rightAtrialPressure, setRightAtrialPressure] = useState(10);
      const [minimumHeartRate, setMinimumHeartRate] = useState(60);
      const [flowLimit] = useState(0.0);
      const [leftPhysOutput] = useState(0.0);
      const [rtPhysOutput] = useState(0.0);
    
      const handleIncrement = (setter, value) => {
        setter(value + 1);
      };
    
      const handleDecrement = (setter, value) => {
        setter(value - 1);
      };
    
      const updateSetpoints = () => {
        // This would call your external Update Setpoints method
        console.log('Updating setpoints with:', {
          leftAtrialPressure,
          rightAtrialPressure,
          minimumHeartRate,
          flowLimit
        });
      };
    
      return (
        <div className="bg-neutral-900 text-gray-100 mt-0 p-3 rounded-lg shadow-lg w-full">
          <h2 className={`text-lg font-bold mb-4 mt-0 text-center ${disabled ? 'text-neutral-500' : 'text-white'}`}>
            Physiological Control Parameters{disabled ? " (inactive)" : ""}
          </h2>
          
          <div className="grid grid-cols-4 gap-1">
            {/* Left Atrial Pressure */}
            <div className="flex flex-col">
              <label className="text-xs font-medium text-gray-300 mb-1">
                LAP Target
              </label>
              <div className="flex">
                <input
                  type="number"
                  value={leftAtrialPressure}
                  onChange={(e) => setLeftAtrialPressure(parseInt(e.target.value) || 0)}
                  className="bg-neutral-900 border border-neutral-700 text-white py-0 px-2 rounded-l w-16 text-center focus:outline-none [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none disabled:opacity-50 disabled:cursor-not-allowed"
                  disabled={disabled}
                />
                <div className="flex flex-col">
                  <button
                    onClick={() => handleIncrement(setLeftAtrialPressure, leftAtrialPressure)}
                    className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-tr border-t border-r border-neutral-600 text-xs disabled:opacity-50 disabled:cursor-not-allowed disabled:hover:bg-neutral-600"
                    disabled={disabled}
                  >
                    ▲
                  </button>
                  <button
                    onClick={() => handleDecrement(setLeftAtrialPressure, leftAtrialPressure)}
                    className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-br border-b border-r border-neutral-600 text-xs disabled:opacity-50 disabled:cursor-not-allowed disabled:hover:bg-neutral-600"
                    disabled={disabled}
                  >
                    ▼
                  </button>
                </div>
              </div>
            </div>
    
            {/* Right Atrial Pressure */}
            <div className="flex flex-col">
              <label className="text-xs font-medium text-gray-300 mb-1">
                RAP Target
              </label>
              <div className="flex">
                <input
                  type="number"
                  value={rightAtrialPressure}
                  onChange={(e) => setRightAtrialPressure(parseInt(e.target.value) || 0)}
                  className="bg-neutral-900 border border-neutral-700 text-white py-0 px-2 rounded-l w-16 text-center focus:outline-none [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none disabled:opacity-50 disabled:cursor-not-allowed"
                  disabled={disabled}
                />
                <div className="flex flex-col">
                  <button
                    onClick={() => handleIncrement(setRightAtrialPressure, rightAtrialPressure)}
                    className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-tr border-t border-r border-neutral-600 text-xs disabled:opacity-50 disabled:cursor-not-allowed disabled:hover:bg-neutral-600"
                    disabled={disabled}
                  >
                    ▲
                  </button>
                  <button
                    onClick={() => handleDecrement(setRightAtrialPressure, rightAtrialPressure)}
                    className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-br border-b border-r border-neutral-600 text-xs disabled:opacity-50 disabled:cursor-not-allowed disabled:hover:bg-neutral-600"
                    disabled={disabled}
                  >
                    ▼
                  </button>
                </div>
              </div>
            </div>
    
            {/* Minimum Heart Rate - Always enabled */}
            <div className="flex flex-col">
              <label className="text-xs font-medium text-gray-300 mb-1">
                Min Heart Rate
              </label>
              <div className="flex">
                <input
                  type="number"
                  value={minimumHeartRate}
                  onChange={(e) => setMinimumHeartRate(parseInt(e.target.value) || 60)}
                  className="bg-neutral-900 border border-neutral-700 text-white py-0 px-2 rounded-l w-16 text-center focus:outline-none [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none"
                />
                <div className="flex flex-col">
                  <button
                    onClick={() => handleIncrement(setMinimumHeartRate, minimumHeartRate)}
                    className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-tr border-t border-r border-neutral-600 text-xs"
                  >
                    ▲
                  </button>
                  <button
                    onClick={() => handleDecrement(setMinimumHeartRate, minimumHeartRate)}
                    className="bg-neutral-600 hover:bg-neutral-500 text-white px-0 py-0 rounded-br border-b border-r border-neutral-600 text-xs"
                  >
                    ▼
                  </button>
                </div>
              </div>
            </div>
    
            {/* Flow Limit (Disabled) */}
            <div className="flex flex-col">
              <label className="text-xs font-medium text-gray-300 mb-1">
                Flow Limit
              </label>
              <input
                type="text"
                value={flowLimit.toFixed(1)}
                disabled
                className="bg-neutral-700 border border-neutral-600 text-gray-400 py-1 px-2 rounded w-16 text-center focus:outline-none cursor-not-allowed"
              />
            </div>
    
            {/* Set Button - spans full width at bottom */}
            <div className="col-span-4 mt-2">
            <div className="flex justify-center">
              <button
                onClick={updateSetpoints}
                className="w-1/2 mx-auto bg-neutral-800 border-neutral-600 hover:bg-neutral-800 text-white font-medium py-1 px-4 rounded-md shadow-sm focus:outline-none transition-colors duration-200"
              >
                Set Parameters
              </button>
              </div>
            </div>
          </div>

          {useMemo(() => (
            <Accordion type="single" collapsible className="w-full mt-2">
              <AccordionItem value="outputs">
                <AccordionTrigger className="text-neutral-500 font-medium py-2">Controller Outputs</AccordionTrigger>
                <AccordionContent className="pt-0 pb-2">
                  <div className="grid grid-cols-2 gap-4 px-1">
                    <div className="flex flex-col">
                      <label className="text-xs font-medium text-gray-300 mb-1">
                        Left Phys Output
                      </label>
                      <input
                        type="text"
                        value={leftPhysOutput.toFixed(1)}
                        disabled
                        className="bg-neutral-700 border border-neutral-600 text-gray-400 py-1 px-2 rounded w-16 text-center focus:outline-none cursor-not-allowed"
                      />
                    </div>

                    <div className="flex flex-col">
                      <label className="text-xs font-medium text-gray-300 mb-1">
                        Rt Phys Output
                      </label>
                      <input
                        type="text"
                        value={rtPhysOutput.toFixed(1)}
                        disabled
                        className="bg-neutral-700 border border-neutral-600 text-gray-400 py-1 px-2 rounded w-16 text-center focus:outline-none cursor-not-allowed"
                      />
                    </div>
                  </div>
                </AccordionContent>
              </AccordionItem>
            </Accordion>
          ), [])}

        </div>
      );
    };

    export default PhysiologicalControl;