
const GraphitePattern = () => {
  const containerStyle = {
    display: 'flex',
    flexDirection: 'column',
    gap: '20px',
    padding: '20px',
    background: '#f5f5f5',
    borderRadius: '8px',
    maxWidth: '800px'
  };

  const exampleStyle = {
    width: '100%',
    height: '120px',
    borderRadius: '6px',
    boxShadow: '0 4px 6px rgba(0,0,0,0.1)',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    color: 'white',
    fontWeight: 'bold',
    fontSize: 'lg'
  };

  // Different graphite-like patterns
  const patterns = [
    {
      name: "Fine Grain Graphite",
      style: {
        background: `
          linear-gradient(45deg, #2c2c2c 25%, transparent 25%, transparent 75%, #2c2c2c 75%),
          linear-gradient(45deg, #2c2c2c 25%, transparent 25%, transparent 75%, #2c2c2c 75%)
        `,
        backgroundColor: '#333',
        backgroundSize: '4px 4px',
        backgroundPosition: '0 0, 2px 2px'
      }
    },
    {
      name: "Carbon Fiber Effect",
      style: {
        background: `
          linear-gradient(27deg, #222 5px, transparent 5px) 0 5px,
          linear-gradient(207deg, #222 5px, transparent 5px) 10px 0,
          linear-gradient(27deg, #222 5px, transparent 5px) 0 10px,
          linear-gradient(207deg, #222 5px, transparent 5px) 10px 5px
        `,
        backgroundColor: '#333',
        backgroundSize: '20px 20px',
        backgroundPosition: '0 0'
      }
    },
    {
      name: "Metallic Graphite",
      style: {
        background: 
          `linear-gradient(135deg, 
            #3a3a3a 0%, 
            #2a2a2a 40%, 
            #2d2d2d 45%, 
            #1e1e1e 50%, 
            #2d2d2d 55%, 
            #2a2a2a 60%, 
            #3a3a3a 100%)`,
        backgroundSize: '10px 10px'
      }
    },
    {
      name: "Brushed Graphite",
      style: {
        background: 
          `linear-gradient(90deg, 
            #333 0px, 
            #333 1px, 
            #383838 1px, 
            #383838 2px, 
            #333 2px, 
            #333 3px, 
            #2e2e2e 3px, 
            #2e2e2e 4px)`,
        backgroundSize: '4px 4px'
      }
    },
    {
      name: "Noise Texture (Simulated)",
      style: {
        position: 'relative',
        backgroundColor: '#333',
        overflow: 'hidden'
      }
    }
  ];

  // Special case for the noise texture which uses a pseudo-element approach
  const noiseStyle = {
    position: 'absolute',
    top: 0,
    right: 0,
    bottom: 0,
    left: 0,
    background: 'url("data:image/svg+xml,%3Csvg viewBox=\'0 0 200 200\' xmlns=\'http://www.w3.org/2000/svg\'%3E%3Cfilter id=\'noiseFilter\'%3E%3CfeTurbulence type=\'fractalNoise\' baseFrequency=\'0.65\' numOctaves=\'3\' stitchTiles=\'stitch\'/%3E%3C/filter%3E%3Crect width=\'100%25\' height=\'100%25\' filter=\'url(%23noiseFilter)\'/%3E%3C/svg%3E")',
    opacity: 0.1,
    mixBlendMode: 'overlay'
  };

  return (
    <div style={containerStyle}>
      <h2 style={{fontSize: 'xl', marginBottom: '10px'}}>CSS Graphite Pattern Examples</h2>
      
      {patterns.map((pattern, index) => (
        <div key={index} style={{width: '100%'}}>
          <h3 style={{marginBottom: '8px'}}>{pattern.name}</h3>
          <div 
            style={{...exampleStyle, ...pattern.style}}
          >
            {pattern.name === "Noise Texture (Simulated)" && (
              <div style={noiseStyle}></div>
            )}
          </div>
        </div>
      ))}
    </div>
  );
};

export default GraphitePattern;