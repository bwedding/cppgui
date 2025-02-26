You have three ways to opt out of reporting:

Add noReport prop to any wrapped component:

jsxCopy<Input noReport type="text" /> // Won't report anything

Use regular HTML elements instead of wrapped ones:

jsxCopy<button onClick={handleClick}>Regular Button</button> // No reporting

Create a non-reporting section using regular components:

jsxCopyconst NavMenu = () => {
  return (
    <nav>
      <button>Home</button>
      <button>About</button>
      {/* None of these report since they're regular HTML elements */}
    </nav>
  );
};
This gives you fine-grained control over what gets reported while keeping the automatic nature of the system for components that should report.


import React, { createContext, useContext, useState } from 'react';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';

const InputHandlerContext = createContext(null);

export const InputHandlerProvider = ({ children }) => {
  const handleInput = (source, data) => {
    console.log(`Input from ${source}:`, data);
  };

  return (
    <InputHandlerContext.Provider value={handleInput}>
      {children}
    </InputHandlerContext.Provider>
  );
};

const useInputHandler = () => {
  const handler = useContext(InputHandlerContext);
  if (!handler) {
    throw new Error('useInputHandler must be used within an InputHandlerProvider');
  }
  return handler;
};

const extractInputData = (element) => {
  const baseData = {
    name: element.name,
    type: element.type,
    id: element.id,
  };

  switch (element.type) {
    case 'checkbox':
      return {
        ...baseData,
        checked: element.checked,
        value: element.value,
      };
    case 'radio':
      return {
        ...baseData,
        checked: element.checked,
        value: element.value,
        group: element.name,
      };
    case 'select-one':
    case 'select-multiple':
      return {
        ...baseData,
        value: element.value,
        selectedOptions: Array.from(element.selectedOptions).map(opt => ({
          value: opt.value,
          text: opt.text,
        })),
      };
    case 'submit':
    case 'button':
      return {
        ...baseData,
        value: element.value || element.textContent,
      };
    default:
      return {
        ...baseData,
        value: element.value,
      };
  }
};

// Default event mapping for different input types
const DEFAULT_EVENTS = {
  checkbox: ['onChange'],
  radio: ['onChange'],
  text: ['onBlur'],
  number: ['onBlur'],
  select: ['onChange'],
  textarea: ['onChange'],
  button: ['onClick'],
  submit: ['onClick'],
  default: ['onChange']
};

const useAutoInput = (props = {}) => {
  const onInput = useInputHandler();
  
  // Skip all reporting if noReport is true
  if (props.noReport) {
    return props;
  }
  
  // Determine which events to listen to
  const reportOn = props.reportOn || DEFAULT_EVENTS[props.type] || DEFAULT_EVENTS.default;
  
  const createHandler = (eventName) => (e) => {
    // Only report if this event type should be reported
    if (reportOn.includes(eventName)) {
      const data = extractInputData(e.target);
      
      if (props.inputData) {
        Object.assign(data, 
          typeof props.inputData === 'function' 
            ? props.inputData(e) 
            : props.inputData
        );
      }

      onInput(data.name || eventName, data);
    }
    
    // Always call original handler if it exists
    props[eventName]?.(e);
  };

  // Only attach handlers for specified events
  const handlers = {};
  ['onChange', 'onClick', 'onInput', 'onBlur'].forEach(event => {
    if (reportOn.includes(event)) {
      handlers[event] = createHandler(event);
    }
  });

  return {
    ...props,
    ...handlers
  };
};

const withAutoInput = (WrappedComponent) => {
  return function AutoInputComponent(props) {
    const enhancedProps = useAutoInput(props);
    return <WrappedComponent {...enhancedProps} />;
  };
};

const Input = withAutoInput('input');
const TextArea = withAutoInput('textarea');
const Select = withAutoInput('select');

const InputDemo = () => {
  return (
    <InputHandlerProvider>
      <Card className="w-full max-w-md">
        <CardHeader>
          <CardTitle>Event-Filtered Input Handler Demo</CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          {/* Text input - reports on blur */}
          <Input 
            type="text"
            name="username"
            placeholder="Type and click away to report..."
            className="border rounded p-2 w-full"
          />
          
          {/* Checkbox with custom event reporting */}
          <label className="flex items-center gap-2">
            <Input
              type="checkbox"
              name="subscribe"
              className="w-4 h-4"
              reportOn={['onChange']} // Explicitly only report onChange
            />
            Subscribe (reports only on change)
          </label>

          {/* Button with custom event reporting */}
          <CustomButton />

          {/* Navigation button that doesn't report */}
          <Input 
            type="button"
            value="Nav Link"
            noReport
            className="bg-gray-200 px-3 py-1 rounded"
            onClick={() => console.log('Navigation only, no reporting')}
          />

          {/* Or use regular HTML elements to avoid reporting */}
          <button 
            className="bg-gray-200 px-3 py-1 rounded ml-2"
            onClick={() => console.log('Regular button, no reporting')}
          >
            Regular Button
          </button>

          <div className="text-sm text-gray-500 mt-4">
            Check the console - some inputs don't report!
          </div>
        </CardContent>
      </Card>
    </InputHandlerProvider>
  );
};

const CustomButton = () => {
  const [count, setCount] = useState(0);
  const props = useAutoInput({
    name: 'custom-counter',
    reportOn: ['onClick'], // Only report click events
    inputData: (e) => ({
      clickCount: count + 1,
      timestamp: new Date().toISOString(),
    }),
    onClick: () => setCount(c => c + 1)
  });

  return (
    <button 
      {...props}
      className="bg-blue-500 text-white px-4 py-2 rounded hover:bg-blue-600"
    >
      Click Counter: {count}
    </button>
  );
};

export default InputDemo;


// Example
         <button 
            onClick={async (event) => {
              const startTime = performance.now();

              try {
                const clickData = {
                  type: "click",
                  timestamp: new Date().toISOString(),
                  x: event.clientX,
                  y: event.clientY,
                  data: "Button clicked!"
                };
                const jsonData = JSON.stringify(clickData);
                const result = window.chrome.webview.hostObjects.sync.native.SendClick(jsonData);
                // async works but is much slower, from 10ms to 300ms
                //const result = await window.chrome.webview.hostObjects.native.SendClick(jsonData);

                if (result === null) {
                  console.log("SendClick sent:", jsonData);
                } else {
                  console.log("SendClick failed with error code:", result);
                }
              } catch (error) {
                console.error("Error sending click:", error);
              }
            }}
            className="px-4 py-2 bg-blue-500 text-white rounded hover:bg-blue-600"
          >
            Click Me
          </button>
