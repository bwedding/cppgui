import Home from './components/home';
import { useEventStore } from './lib/EventStore.ts';
import { useEffect } from 'react';
import { initializeWebView2Listener } from './lib/datastore.ts';

function App() {
  const initialize = useEventStore(state => state.initialize);
  
  useEffect(() => {
    initialize();

    initializeWebView2Listener();

    return () => {
    };
  }, [initialize]);
  
  return (
    <Home />
  );
}

export default App;
