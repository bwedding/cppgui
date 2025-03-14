import PropTypes from 'prop-types';

const LEDBar = ({
  value,
  ledTotal,
  orientation,
  ranges,
  height = "20px",
  width = "100%",
  bezelsColor = "#333", // Dark gray default bezel color (mostly affects non-metallic styles)
  bezelStyle = "classic" // classic, metallic, or plastic
}) => {
  const leds = Array.from(Array(ledTotal).keys());
  
  const orientationAsDirection =
    orientation === "vertical" ? "column-reverse" : "row";
  
  const getLEDColorForIndex = (index) => {
    const matchingRange = ranges.filter((range) => {
      return index >= range.min && index <= range.max;
    });
    
    return matchingRange[0];
  };

  // Determine bezel styling based on the bezelStyle prop
  let bezelStyleObj = {};
  switch (bezelStyle) {
    case 'metallic':
      bezelStyleObj = {
        background: `linear-gradient(135deg, 
          #3a3a3a 0%, 
          #2a2a2a 40%, 
          #2d2d2d 45%, 
          #1e1e1e 50%, 
          #2d2d2d 55%, 
          #2a2a2a 60%, 
          #3a3a3a 100%)`,
        backgroundSize: '10px 10px',
        boxShadow: "0 1px 4px rgba(0,0,0,0.3), 0 0 30px rgba(0,0,0,0.1) inset"
      };
      break;
    case 'plastic':
      bezelStyleObj = {
        background: bezelsColor,
        boxShadow: "0 1px 4px rgba(0,0,0,0.2)",
        borderRadius: "4px"
      };
      break;
    case 'classic':
    default:
      bezelStyleObj = {
        background: bezelsColor,
        boxShadow: "inset 0 1px 3px rgba(0,0,0,0.4), 0 1px 2px rgba(255,255,255,0.1)"
      };
  }
  
  return (
    <div
      style={{
        display: "flex",
        justifyContent: "center",
        alignItems: "center",
        padding: "8px",
        borderRadius: "2px",
        width,
        ...bezelStyleObj
      }}
    >
      <div
        style={{
          height,
          width: "100%",
          display: "flex",
          flexDirection: orientationAsDirection,
          backgroundColor: "#111", // Dark background for LEDs
          padding: "2px",
          borderRadius: "2px",
          boxShadow: "inset 0 0 5px rgba(0,0,0,0.5)"
        }}
      >
        {leds.map((led, idx) => {
          const ledRange = getLEDColorForIndex(idx);
          const isActive = value >= idx;
          
          // Calculate LED styling
          const ledColor = ledRange?.color ?? "grey";
          
          return (
            <span
              key={idx}
              style={{
                minHeight: "100%",
                background: ledColor,
                margin: "1px",
                flexGrow: 1,
                opacity: isActive ? 1 : 0.08,
                boxShadow: isActive 
                  ? `0 0 3px ${ledColor}, 0 0 5px ${ledColor}` 
                  : "none",
                transition: "opacity 0.1s ease, box-shadow 0.1s ease",
                borderRadius: "1px",
                position: "relative"
              }}
            >
              {/* Glass effect overlay */}
              <span
                style={{
                  position: "absolute",
                  top: 0,
                  left: 0,
                  right: 0,
                  bottom: 0,
                  background: "linear-gradient(180deg, rgba(255,255,255,0.15) 0%, rgba(255,255,255,0) 50%, rgba(0,0,0,0.1) 100%)",
                  borderRadius: "1px"
                }}
              />
            </span>
          );
        })}
      </div>
    </div>
  );
};

LEDBar.propTypes = {
  value: PropTypes.number.isRequired,
  ledTotal: PropTypes.number.isRequired,
  orientation: PropTypes.oneOf(['horizontal', 'vertical']),
  ranges: PropTypes.arrayOf(
    PropTypes.shape({
      min: PropTypes.number.isRequired,
      max: PropTypes.number.isRequired,
      color: PropTypes.string.isRequired
    })
  ).isRequired,
  height: PropTypes.string,
  width: PropTypes.string,
  bezelsColor: PropTypes.string,
  bezelStyle: PropTypes.oneOf(['classic', 'metallic', 'plastic'])
};

LEDBar.defaultProps = {
  orientation: 'horizontal',
  height: '20px',
  width: '100%',
  bezelsColor: '#333',
  bezelStyle: 'classic'
};

export default LEDBar;