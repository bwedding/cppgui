import {
  __commonJS,
  require_react
} from "./chunk-3ARHY7UZ.js";

// node_modules/canvas-gauges/gauge.min.js
var require_gauge_min = __commonJS({
  "node_modules/canvas-gauges/gauge.min.js"(exports, module) {
    !function(e) {
      "use strict";
      function t(e2) {
        if (Array.isArray(e2)) {
          for (var t2 = 0, i2 = Array(e2.length); t2 < e2.length; t2++) i2[t2] = e2[t2];
          return i2;
        }
        return Array.from(e2);
      }
      function i(e2, t2) {
        if (!e2) throw new ReferenceError("this hasn't been initialised - super() hasn't been called");
        return !t2 || "object" != typeof t2 && "function" != typeof t2 ? e2 : t2;
      }
      function r(e2, t2) {
        if ("function" != typeof t2 && null !== t2) throw new TypeError("Super expression must either be null or a function, not " + typeof t2);
        e2.prototype = Object.create(t2 && t2.prototype, { constructor: { value: e2, enumerable: false, writable: true, configurable: true } }), t2 && (Object.setPrototypeOf ? Object.setPrototypeOf(e2, t2) : e2.__proto__ = t2);
      }
      function o(e2, t2) {
        if (!(e2 instanceof t2)) throw new TypeError("Cannot call a class as a function");
      }
      function n(e2, t2) {
        if (t2 || (t2 = "undefined" == typeof window ? global : window), void 0 !== t2[e2]) return t2[e2];
        for (var i2 = ["webkit", "moz", "ms", "o"], r2 = 0, o2 = i2.length, n2 = e2.charAt(0).toUpperCase() + e2.substr(1); r2 < o2; r2++) {
          var a2 = t2[i2[r2] + n2];
          if (void 0 !== a2) return a2;
        }
        return null;
      }
      function a(e2, t2, i2, r2, o2, n2, l2) {
        if ("function" != typeof r2) throw new TypeError("Invalid animation rule:", r2);
        var s2 = e2 - i2, d2 = s2 / o2, c2 = 0;
        d2 > 1 && (d2 = 1), 1 !== d2 && (c2 = r2(d2), isFinite(c2) && !isNaN(c2) && (d2 = c2)), t2 && t2(d2), s2 < o2 ? l2.frame = me(function(e3) {
          return a(e3, t2, i2, r2, o2, n2, l2);
        }) : (n2 && n2(), l2.inProgress = false);
      }
      function l() {
        Array.prototype.constructor.apply(this, arguments);
      }
      function s(e2) {
        if (!(e2 instanceof DOMException && 2152923147 === e2.result)) throw e2;
      }
      function d(e2, t2) {
        return t2.replace(Ve, function(t3, i2) {
          var r2 = e2[i2];
          return void 0 !== r2 ? r2 : t3;
        });
      }
      function c(e2) {
        return e2.majorTicks instanceof Array || (e2.majorTicks = e2.majorTicks ? [e2.majorTicks] : []), e2.majorTicks.length || (e2.majorTicks.push(We.formatMajorTickNumber(e2.minValue, e2)), e2.majorTicks.push(We.formatMajorTickNumber(e2.maxValue, e2))), ["right" !== e2.tickSide, "left" !== e2.tickSide];
      }
      function h(e2, t2, i2, r2, o2, n2) {
        e2.beginPath(), e2.moveTo(t2 + n2, i2), e2.lineTo(t2 + r2 - n2, i2), e2.quadraticCurveTo(t2 + r2, i2, t2 + r2, i2 + n2), e2.lineTo(t2 + r2, i2 + o2 - n2), e2.quadraticCurveTo(t2 + r2, i2 + o2, t2 + r2 - n2, i2 + o2), e2.lineTo(t2 + n2, i2 + o2), e2.quadraticCurveTo(t2, i2 + o2, t2, i2 + o2 - n2), e2.lineTo(t2, i2 + n2), e2.quadraticCurveTo(t2, i2, t2 + n2, i2), e2.closePath();
      }
      function u(e2, t2) {
        var i2 = t2.valueDec, r2 = t2.valueInt, o2 = 0, n2 = void 0, a2 = void 0, l2 = void 0;
        if (e2 = parseFloat(e2), l2 = e2 < 0, e2 = Math.abs(e2), i2 > 0) {
          for (a2 = e2.toFixed(i2).toString().split("."), n2 = r2 - a2[0].length; o2 < n2; ++o2) a2[0] = "0" + a2[0];
          a2 = (l2 ? "-" : "") + a2[0] + "." + a2[1];
        } else {
          for (a2 = Math.round(e2).toString(), n2 = r2 - a2.length; o2 < n2; ++o2) a2 = "0" + a2;
          a2 = (l2 ? "-" : "") + a2;
        }
        return a2;
      }
      function f(e2, t2) {
        var i2 = void 0, r2 = false;
        return i2 = 0 === t2.majorTicksDec ? Math.round(e2).toString() : e2.toFixed(t2.majorTicksDec), t2.majorTicksInt > 1 ? (r2 = ~i2.indexOf("."), ~i2.indexOf("-") ? "-" + [t2.majorTicksInt + t2.majorTicksDec + 2 + (r2 ? 1 : 0) - i2.length].join("0") + i2.replace("-", "") : [t2.majorTicksInt + t2.majorTicksDec + 1 + (r2 ? 1 : 0) - i2.length].join("0") + i2) : i2;
      }
      function m(e2) {
        return e2 * Math.PI / 180;
      }
      function v(e2, t2) {
        return { x: -e2 * Math.sin(t2), y: e2 * Math.cos(t2) };
      }
      function g(e2, t2, i2, r2) {
        var o2 = !(arguments.length > 4 && void 0 !== arguments[4]) || arguments[4], n2 = arguments.length > 5 && void 0 !== arguments[5] ? arguments[5] : 0, a2 = e2.createLinearGradient(o2 ? 0 : n2, o2 ? n2 : 0, o2 ? 0 : r2, o2 ? r2 : 0);
        return a2.addColorStop(0, t2), a2.addColorStop(1, i2), a2;
      }
      function b(e2, t2) {
        if (arguments.length > 2 && void 0 !== arguments[2] && arguments[2]) return e2.restore(), true;
        e2.save();
        var i2 = t2.borderShadowWidth;
        return i2 && (e2.shadowBlur = i2, e2.shadowColor = t2.colorBorderShadow), true;
      }
      function p(e2, t2) {
        t2.needleShadow && (e2.shadowOffsetX = 2, e2.shadowOffsetY = 2, e2.shadowBlur = 10, e2.shadowColor = t2.colorNeedleShadowDown);
      }
      function w(e2, t2, i2) {
        return e2["font" + t2 + "Style"] + " " + e2["font" + t2 + "Weight"] + " " + e2["font" + t2 + "Size"] * i2 + "px " + e2["font" + t2];
      }
      function k(e2) {
        e2.shadowOffsetX = null, e2.shadowOffsetY = null, e2.shadowBlur = null, e2.shadowColor = "", e2.strokeStyle = null, e2.lineWidth = 0, e2.save();
      }
      function y(e2, t2, i2, r2) {
        t2.valueTextShadow && (e2.shadowOffsetX = i2, e2.shadowOffsetY = i2, e2.shadowBlur = r2, e2.shadowColor = t2.colorValueTextShadow);
      }
      function x(e2, t2, i2, r2, o2, n2) {
        if (t2.valueBox) {
          k(e2);
          var a2 = t2.valueDec ? 1 + t2.valueDec : 0, l2 = "9".repeat(Math.max.apply(null, [String(parseInt(i2)).length + a2].concat(t2.majorTicks.map(function(e3) {
            return String(parseInt(e3, 10)).length + a2;
          })))), s2 = t2.valueText || u(i2, t2), d2 = n2 / 200, c2 = n2 / 100, f2 = 0.4 * c2, m2 = 1.2 * c2;
          e2.font = w(t2, "Value", d2), y(e2, t2, f2, m2);
          var v2 = e2.measureText(t2.valueText ? s2 : "-" + u(Number(l2), t2)).width;
          k(e2);
          var g2 = parseFloat(t2.fontValueSize) * d2 + f2 + m2, b2 = c2 * parseFloat(t2.valueBoxStroke), p2 = 2 * n2 - 2 * b2, x2 = v2 + 10 * c2, T2 = 1.1 * g2 + f2 + m2, S2 = c2 * t2.valueBoxBorderRadius, V2 = (parseFloat(t2.valueBoxWidth) || 0) / 100 * p2;
          V2 > x2 && (x2 = V2), x2 > p2 && (x2 = p2);
          var W2 = r2 - x2 / 2, O2 = o2 - T2 / 2, A2 = o2 - 5.75 * c2;
          if (e2.beginPath(), S2 ? h(e2, W2, O2, x2, T2, S2) : e2.rect(W2, O2, x2, T2), b2) {
            var P2 = e2.createRadialGradient(r2, A2, 10 * c2, r2, A2, 20 * c2);
            P2.addColorStop(0, t2.colorValueBoxRect), P2.addColorStop(1, t2.colorValueBoxRectEnd), e2.strokeStyle = P2, e2.lineWidth = b2, e2.stroke();
          }
          t2.colorValueBoxShadow && (e2.shadowBlur = 1.2 * c2, e2.shadowColor = t2.colorValueBoxShadow), t2.colorValueBoxBackground && (e2.fillStyle = t2.colorValueBoxBackground, e2.fill()), e2.closePath(), e2.restore(), y(e2, t2, f2, m2), e2.fillStyle = t2.colorValueText, e2.textAlign = "center", e2.textBaseline = "alphabetic", e2.fillText(s2, W2 + x2 / 2, o2 + T2 / 2 - g2 / 3), e2.restore();
        }
      }
      function T(e2) {
        var t2 = e2.value, i2 = e2.minValue, r2 = e2.maxValue, o2 = 0.01 * (r2 - i2);
        return { normal: t2 < i2 ? i2 : t2 > r2 ? r2 : t2, indented: t2 < i2 ? i2 - o2 : t2 > r2 ? r2 + o2 : t2 };
      }
      function S(e2, t2, i2, r2, o2) {
        i2.beginPath(), i2.arc(0, 0, xe(e2), 0, 2 * Oe, true), i2.lineWidth = t2, i2.strokeStyle = o2 ? We.linearGradient(i2, r2, o2, e2) : r2, i2.stroke(), i2.closePath();
      }
      function V(e2, t2) {
        var i2 = pe.pixelRatio;
        return e2.maxRadius || (e2.maxRadius = e2.max - t2.borderShadowWidth - t2.borderOuterWidth * i2 - t2.borderMiddleWidth * i2 - t2.borderInnerWidth * i2 + (t2.borderOuterWidth ? 0.5 : 0) + (t2.borderMiddleWidth ? 0.5 : 0) + (t2.borderInnerWidth ? 0.5 : 0)), e2.maxRadius;
      }
      function W(e2, t2) {
        var i2 = pe.pixelRatio, r2 = t2.borderShadowWidth * i2, o2 = e2.max - r2 - t2.borderOuterWidth * i2 / 2, n2 = o2 - t2.borderOuterWidth * i2 / 2 - t2.borderMiddleWidth * i2 / 2 + 0.5, a2 = n2 - t2.borderMiddleWidth * i2 / 2 - t2.borderInnerWidth * i2 / 2 + 0.5, l2 = V(e2, t2), s2 = void 0, d2 = false;
        e2.save(), t2.borderOuterWidth && (d2 = We.drawShadow(e2, t2, d2), S(o2, t2.borderOuterWidth * i2, e2, t2.colorBorderOuter, t2.colorBorderOuterEnd)), t2.borderMiddleWidth && (d2 = We.drawShadow(e2, t2, d2), S(n2, t2.borderMiddleWidth * i2, e2, t2.colorBorderMiddle, t2.colorBorderMiddleEnd)), t2.borderInnerWidth && (d2 = We.drawShadow(e2, t2, d2), S(a2, t2.borderInnerWidth * i2, e2, t2.colorBorderInner, t2.colorBorderInnerEnd)), We.drawShadow(e2, t2, d2), e2.beginPath(), e2.arc(0, 0, xe(l2), 0, 2 * Oe, true), t2.colorPlateEnd ? (s2 = e2.createRadialGradient(0, 0, l2 / 2, 0, 0, l2), s2.addColorStop(0, t2.colorPlate), s2.addColorStop(1, t2.colorPlateEnd)) : s2 = t2.colorPlate, e2.fillStyle = s2, e2.fill(), e2.closePath(), e2.restore();
      }
      function O(e2, t2) {
        var i2 = e2.max * (parseFloat(t2.highlightsWidth) || 0) / 100;
        if (i2) {
          var r2 = xe(P(e2, t2) - i2 / 2), o2 = 0, n2 = t2.highlights.length, a2 = (t2.maxValue - t2.minValue) / t2.ticksAngle;
          for (e2.save(); o2 < n2; o2++) {
            var l2 = t2.highlights[o2];
            e2.beginPath(), e2.rotate(Ae), e2.arc(0, 0, r2, We.radians(t2.startAngle + (l2.from - t2.minValue) / a2), We.radians(t2.startAngle + (l2.to - t2.minValue) / a2), false), e2.strokeStyle = l2.color, e2.lineWidth = i2, e2.lineCap = t2.highlightsLineCap, e2.stroke(), e2.closePath(), e2.restore(), e2.save();
          }
        }
      }
      function A(e2, t2) {
        var i2 = P(e2, t2), r2 = void 0, o2 = void 0, n2 = void 0, a2 = 0, l2 = 0, s2 = Math.abs(t2.minorTicks) || 0, d2 = t2.ticksAngle / (t2.maxValue - t2.minValue);
        for (e2.lineWidth = pe.pixelRatio, e2.strokeStyle = t2.colorMinorTicks || t2.colorStrokeTicks, e2.save(), t2.exactTicks ? (o2 = t2.maxValue - t2.minValue, r2 = s2 ? o2 / s2 : 0, l2 = (Se.mod(t2.majorTicks[0], s2) || 0) * d2) : r2 = s2 * (t2.majorTicks.length - 1); a2 < r2; ++a2) (n2 = t2.startAngle + l2 + a2 * (t2.ticksAngle / r2)) <= t2.ticksAngle + t2.startAngle && (e2.rotate(We.radians(n2)), e2.beginPath(), e2.moveTo(0, i2), e2.lineTo(0, i2 - 0.075 * e2.max), C(e2));
      }
      function P(e2, t2) {
        var i2 = e2.max / 100;
        return V(e2, t2) - 5 * i2 - (t2.barWidth ? 2 * (parseFloat(t2.barStrokeWidth) || 0) + ((parseFloat(t2.barWidth) || 0) + 5) * i2 : 0);
      }
      function M(e2, t2) {
        We.prepareTicks(t2);
        var i2 = xe(P(e2, t2)), r2 = void 0, o2 = void 0, n2 = t2.majorTicks.length, a2 = pe.pixelRatio;
        for (e2.lineWidth = 2 * a2, e2.save(), o2 = t2.colorMajorTicks instanceof Array ? t2.colorMajorTicks : new Array(n2).fill(t2.colorStrokeTicks || t2.colorMajorTicks), r2 = 0; r2 < n2; ++r2) e2.strokeStyle = o2[r2], e2.rotate(We.radians(B(t2, t2.exactTicks ? t2.majorTicks[r2] : r2, n2))), e2.beginPath(), e2.moveTo(0, i2), e2.lineTo(0, i2 - 0.15 * e2.max), C(e2);
        t2.strokeTicks && (e2.strokeStyle = t2.colorStrokeTicks || o2[0], e2.rotate(Ae), e2.beginPath(), e2.arc(0, 0, i2, We.radians(t2.startAngle), We.radians(t2.startAngle + t2.ticksAngle), false), C(e2));
      }
      function B(e2, t2, i2) {
        if (e2.exactTicks) {
          var r2 = e2.ticksAngle / (e2.maxValue - e2.minValue);
          return e2.startAngle + r2 * (t2 - e2.minValue);
        }
        return e2.startAngle + t2 * (e2.ticksAngle / (i2 - 1));
      }
      function C(e2) {
        e2.stroke(), e2.restore(), e2.closePath(), e2.save();
      }
      function j(e2, t2) {
        var i2 = P(e2, t2) - 0.15 * e2.max, r2 = {}, o2 = 0, n2 = t2.majorTicks.length, a2 = "needle" !== t2.animationTarget, l2 = t2.colorNumbers instanceof Array ? t2.colorNumbers : new Array(n2).fill(t2.colorNumbers), s2 = a2 ? -(t2.value - t2.minValue) / (t2.maxValue - t2.minValue) * t2.ticksAngle : 0;
        for (a2 && (e2.save(), e2.rotate(-We.radians(s2))), e2.font = We.font(t2, "Numbers", e2.max / 200), e2.lineWidth = 0, e2.textAlign = "center", e2.textBaseline = "middle"; o2 < n2; ++o2) {
          var d2 = s2 + B(t2, t2.exactTicks ? t2.majorTicks[o2] : o2, n2), c2 = e2.measureText(t2.majorTicks[o2]).width, h2 = t2.fontNumbersSize, u2 = Math.sqrt(c2 * c2 + h2 * h2) / 2, f2 = We.radialPoint(i2 - u2 - t2.numbersMargin / 100 * e2.max, We.radians(d2));
          360 === d2 && (d2 = 0), r2[d2] || (r2[d2] = true, e2.fillStyle = l2[o2], e2.fillText(t2.majorTicks[o2], f2.x, f2.y));
        }
        a2 && e2.restore();
      }
      function N(e2, t2) {
        t2.title && (e2.save(), e2.font = We.font(t2, "Title", e2.max / 200), e2.fillStyle = t2.colorTitle, e2.textAlign = "center", e2.fillText(t2.title, 0, -e2.max / 4.25, 0.8 * e2.max), e2.restore());
      }
      function E(e2, t2) {
        t2.units && (e2.save(), e2.font = We.font(t2, "Units", e2.max / 200), e2.fillStyle = t2.colorUnits, e2.textAlign = "center", e2.fillText(We.formatContext(t2, t2.units), 0, e2.max / 3.25, 0.8 * e2.max), e2.restore());
      }
      function _(e2, t2) {
        if (t2.needle) {
          var i2 = t2.ticksAngle < 360 ? We.normalizedValue(t2).indented : t2.value, r2 = u2 ? t2.startAngle : t2.startAngle + (i2 - t2.minValue) / (t2.maxValue - t2.minValue) * t2.ticksAngle;
          "right" === t2.barStartPosition && (r2 = t2.startAngle + t2.ticksAngle - (i2 - t2.minValue) / (t2.maxValue - t2.minValue) * t2.ticksAngle);
          var o2 = V(e2, t2), n2 = xe(o2 / 100 * t2.needleCircleSize), a2 = xe(o2 / 100 * t2.needleCircleSize * 0.75), l2 = xe(o2 / 100 * t2.needleEnd), s2 = xe(t2.needleStart ? o2 / 100 * t2.needleStart : 0), d2 = o2 / 100 * t2.needleWidth, c2 = o2 / 100 * t2.needleWidth / 2, h2 = pe.pixelRatio, u2 = "needle" !== t2.animationTarget;
          e2.save(), We.drawNeedleShadow(e2, t2), e2.rotate(We.radians(r2)), e2.fillStyle = We.linearGradient(e2, t2.colorNeedle, t2.colorNeedleEnd, l2 - s2), "arrow" === t2.needleType ? (e2.beginPath(), e2.moveTo(-c2, -s2), e2.lineTo(-d2, 0), e2.lineTo(-1 * h2, l2), e2.lineTo(h2, l2), e2.lineTo(d2, 0), e2.lineTo(c2, -s2), e2.closePath(), e2.fill(), e2.beginPath(), e2.lineTo(-0.5 * h2, l2), e2.lineTo(-1 * h2, l2), e2.lineTo(-d2, 0), e2.lineTo(-c2, -s2), e2.lineTo(c2 / 2 * h2 - 2 * h2, -s2), e2.closePath(), e2.fillStyle = t2.colorNeedleShadowUp, e2.fill()) : (e2.beginPath(), e2.moveTo(-c2, l2), e2.lineTo(-c2, s2), e2.lineTo(c2, s2), e2.lineTo(c2, l2), e2.closePath(), e2.fill()), t2.needleCircleSize && (e2.restore(), We.drawNeedleShadow(e2, t2), t2.needleCircleOuter && (e2.beginPath(), e2.arc(0, 0, n2, 0, 2 * Oe, true), e2.fillStyle = We.linearGradient(e2, t2.colorNeedleCircleOuter, t2.colorNeedleCircleOuterEnd, n2), e2.fill(), e2.closePath()), t2.needleCircleInner && (e2.beginPath(), e2.arc(0, 0, a2, 0, 2 * Oe, true), e2.fillStyle = We.linearGradient(e2, t2.colorNeedleCircleInner, t2.colorNeedleCircleInnerEnd, a2), e2.fill(), e2.closePath()), e2.restore());
        }
      }
      function R(e2, t2, i2) {
        We.drawValueBox(e2, t2, i2, 0, e2.max - 0.33 * e2.max, e2.max);
      }
      function I(e2) {
        var t2 = e2.startAngle, i2 = e2.startAngle + e2.ticksAngle, r2 = t2, o2 = t2 + (We.normalizedValue(e2).normal - e2.minValue) / (e2.maxValue - e2.minValue) * e2.ticksAngle;
        if ("middle" === e2.barStartPosition) {
          var n2 = 0.5 * (e2.minValue + e2.maxValue);
          e2.value < n2 ? (r2 = 180 - (n2 - We.normalizedValue(e2).normal) / (e2.maxValue - e2.minValue) * e2.ticksAngle, o2 = 180) : (r2 = 180, o2 = 180 + (We.normalizedValue(e2).normal - n2) / (e2.maxValue - e2.minValue) * e2.ticksAngle);
        } else "right" === e2.barStartPosition && (r2 = i2 - o2 + t2, o2 = i2);
        return { startAngle: r2, endAngle: o2 };
      }
      function D(e2, t2) {
        var i2 = e2.max / 100, r2 = V(e2, t2) - 5 * i2, o2 = parseFloat(t2.barStrokeWidth + "") || 0, n2 = (parseFloat(t2.barWidth + "") || 0) * i2, a2 = r2 - 2 * o2 - n2, l2 = (r2 - a2) / 2, s2 = a2 + l2, d2 = o2 / s2, c2 = t2.startAngle, h2 = t2.startAngle + t2.ticksAngle;
        if (e2.save(), e2.rotate(Ae), o2 && (e2.beginPath(), e2.arc(0, 0, s2, We.radians(c2) - d2, We.radians(h2) + d2, false), e2.strokeStyle = t2.colorBarStroke, e2.lineWidth = 2 * l2, e2.stroke(), e2.closePath()), n2 && (e2.beginPath(), e2.arc(0, 0, s2, We.radians(c2), We.radians(h2), false), e2.strokeStyle = t2.colorBar, e2.lineWidth = n2, e2.stroke(), e2.closePath(), t2.barShadow && (e2.beginPath(), e2.arc(0, 0, r2, We.radians(c2), We.radians(h2), false), e2.clip(), e2.beginPath(), e2.strokeStyle = t2.colorBar, e2.lineWidth = 1, e2.shadowBlur = t2.barShadow, e2.shadowColor = t2.colorBarShadow, e2.shadowOffsetX = 0, e2.shadowOffsetY = 0, e2.arc(0, 0, r2, We.radians(t2.startAngle), We.radians(t2.startAngle + t2.ticksAngle), false), e2.stroke(), e2.closePath(), e2.restore(), e2.rotate(Ae)), t2.barProgress)) {
          var u2 = I(t2), f2 = u2.startAngle, m2 = u2.endAngle;
          e2.beginPath(), e2.arc(0, 0, s2, We.radians(f2), We.radians(m2), false), e2.strokeStyle = t2.colorBarProgress, e2.lineWidth = n2, e2.stroke(), e2.closePath();
        }
        e2.restore();
      }
      function z(e2) {
        return e2.options.animatedValue ? e2.options.value : e2.value;
      }
      function L(e2, t2, i2, r2, o2, n2, a2, l2) {
        e2.beginPath(), e2.fillStyle = l2 ? We.linearGradient(e2, a2, l2, o2 > n2 ? o2 : n2, n2 > o2, o2 > n2 ? i2 : r2) : a2, t2 > 0 ? We.roundRect(e2, i2, r2, o2, n2, t2) : e2.rect(i2, r2, o2, n2), e2.fill(), e2.closePath();
      }
      function G(e2, t2, i2, r2, o2, n2, a2, l2, s2) {
        e2.beginPath(), e2.lineWidth = t2, e2.strokeStyle = s2 ? We.linearGradient(e2, l2, s2, a2, true, o2) : l2, i2 > 0 ? We.roundRect(e2, r2, o2, n2, a2, i2) : e2.rect(r2, o2, n2, a2), e2.stroke(), e2.closePath();
      }
      function F(e2, t2, i2, r2, o2, n2) {
        var a2 = pe.pixelRatio;
        e2.save();
        var l2 = t2.borderRadius * a2, s2 = o2 - t2.borderShadowWidth - t2.borderOuterWidth * a2, d2 = s2 - t2.borderOuterWidth * a2 - t2.borderMiddleWidth * a2, c2 = d2 - t2.borderMiddleWidth * a2 - t2.borderInnerWidth * a2, h2 = c2 - t2.borderInnerWidth * a2, u2 = n2 - t2.borderShadowWidth - t2.borderOuterWidth * a2, f2 = u2 - t2.borderOuterWidth * a2 - t2.borderMiddleWidth * a2, m2 = f2 - t2.borderMiddleWidth * a2 - t2.borderInnerWidth * a2, v2 = m2 - t2.borderInnerWidth * a2, g2 = i2 - (d2 - s2) / 2, b2 = g2 - (c2 - d2) / 2, p2 = b2 - (h2 - c2) / 2, w2 = r2 - (f2 - u2) / 2, k2 = w2 - (m2 - f2) / 2, y2 = k2 - (v2 - m2) / 2, x2 = 0, T2 = false;
        return t2.borderOuterWidth && (T2 = We.drawShadow(e2, t2, T2), G(e2, t2.borderOuterWidth * a2, l2, i2 + t2.borderOuterWidth * a2 / 2 - x2, r2 + t2.borderOuterWidth * a2 / 2 - x2, s2, u2, t2.colorBorderOuter, t2.colorBorderOuterEnd), x2 += 0.5 * a2), t2.borderMiddleWidth && (T2 = We.drawShadow(e2, t2, T2), G(e2, t2.borderMiddleWidth * a2, l2 -= 1 + 2 * x2, g2 + t2.borderMiddleWidth * a2 / 2 - x2, w2 + t2.borderMiddleWidth * a2 / 2 - x2, d2 + 2 * x2, f2 + 2 * x2, t2.colorBorderMiddle, t2.colorBorderMiddleEnd), x2 += 0.5 * a2), t2.borderInnerWidth && (T2 = We.drawShadow(e2, t2, T2), G(e2, t2.borderInnerWidth * a2, l2 -= 1 + 2 * x2, b2 + t2.borderInnerWidth * a2 / 2 - x2, k2 + t2.borderInnerWidth * a2 / 2 - x2, c2 + 2 * x2, m2 + 2 * x2, t2.colorBorderInner, t2.colorBorderInnerEnd), x2 += 0.5 * a2), We.drawShadow(e2, t2, T2), L(e2, l2, p2, y2, h2 + 2 * x2, v2 + 2 * x2, t2.colorPlate, t2.colorPlateEnd), e2.restore(), [p2, y2, h2, v2];
      }
      function X(e2, t2, i2, r2, o2, n2) {
        var a2 = pe.pixelRatio, l2 = n2 >= o2, s2 = l2 ? 0.85 * o2 : n2, d2 = l2 ? n2 : o2;
        i2 = l2 ? ye(i2 + (o2 - s2) / 2) : i2;
        var c2 = !!t2.title, h2 = !!t2.units, u2 = !!t2.valueBox, f2 = void 0, m2 = void 0, v2 = void 0;
        l2 ? (m2 = ye(0.05 * d2), f2 = ye(0.075 * d2), v2 = ye(0.11 * d2), c2 && (d2 -= f2, r2 += f2), h2 && (d2 -= m2), u2 && (d2 -= v2)) : (m2 = f2 = ye(0.15 * s2), c2 && (s2 -= f2, r2 += f2), h2 && (s2 -= m2));
        var g2 = 2 * t2.barStrokeWidth, b2 = t2.barBeginCircle ? ye(s2 * t2.barBeginCircle / 200 - g2 / 2) : 0, p2 = ye(s2 * t2.barWidth / 100 - g2), w2 = ye(d2 * t2.barLength / 100 - g2), k2 = ye((d2 - w2) / 2), y2 = ye(i2 + (l2 ? s2 / 2 : k2 + b2)), x2 = ye(r2 + (l2 ? d2 - k2 - b2 + g2 / 2 : s2 / 2)), T2 = !l2 || t2.hasLeft && t2.hasRight ? 0 : (t2.hasRight ? -1 : 1) * t2.ticksWidth / 100 * s2, S2 = l2 || t2.hasLeft && t2.hasRight ? 0 : (t2.hasRight ? -1 : 1) * t2.ticksWidth / 100 * s2;
        return e2.barDimensions = { isVertical: l2, width: s2, length: d2, barWidth: p2, barLength: w2, strokeWidth: g2, barMargin: k2, radius: b2, pixelRatio: a2, barOffset: null, titleMargin: c2 ? f2 : 0, unitsMargin: h2 ? m2 : 0, get ticksLength() {
          return this.barLength - this.barOffset - this.strokeWidth;
        }, X: i2 + T2, Y: r2 + S2, x0: y2 + T2, y0: x2 + S2, baseX: i2, baseY: r2, ticksPadding: t2.ticksPadding / 100 }, e2.barDimensions;
      }
      function Y(e2, t2, i2, r2, o2, n2, a2) {
        var l2 = X(e2, t2, r2, o2, n2, a2), s2 = l2.isVertical, d2 = l2.width, c2 = l2.barWidth, h2 = l2.barLength, u2 = l2.strokeWidth, f2 = l2.barMargin, m2 = l2.radius, v2 = l2.x0, g2 = l2.y0, b2 = l2.X, p2 = l2.Y, w2 = h2;
        if (e2.save(), e2.beginPath(), t2.barBeginCircle) {
          var k2 = We.radians(s2 ? 270 : 0), y2 = Math.asin(c2 / 2 / m2), x2 = Math.cos(y2), T2 = Math.sin(y2), S2 = v2 + (s2 ? m2 * T2 : m2 * x2 - u2 / 2), V2 = s2 ? g2 - m2 * x2 : g2 + m2 * T2, W2 = xe(s2 ? V2 - g2 : S2 - v2);
          e2.barDimensions.barOffset = ye(W2 + m2);
          var O2 = s2 ? ye(v2 - m2 * T2) : S2, A2 = s2 ? V2 : ye(g2 - m2 * T2);
          "progress" === i2 && (h2 = e2.barDimensions.barOffset + (h2 - e2.barDimensions.barOffset) * (We.normalizedValue(t2).normal - t2.minValue) / (t2.maxValue - t2.minValue));
          var P2 = ye(S2 + h2 - e2.barDimensions.barOffset + u2 / 2), M2 = ye(V2 - h2 + e2.barDimensions.barOffset - u2 / 2);
          e2.arc(v2, g2, m2, k2 + y2, k2 - y2), s2 ? (e2.moveTo(S2, A2), e2.lineTo(S2, M2), e2.lineTo(O2, M2), e2.lineTo(O2, A2)) : (e2.moveTo(S2, A2), e2.lineTo(P2, A2), e2.lineTo(P2, V2), e2.lineTo(S2, V2));
        } else {
          var B2 = ye(s2 ? b2 + (d2 - c2) / 2 : b2 + f2), C2 = ye(s2 ? p2 + h2 + f2 : p2 + (d2 - c2) / 2);
          "progress" === i2 && (h2 *= (t2.value - t2.minValue) / (t2.maxValue - t2.minValue)), s2 ? e2.rect(B2, C2, c2, -h2) : e2.rect(B2, C2, h2, c2);
        }
        "progress" !== i2 && t2.barStrokeWidth && (e2.lineWidth = u2, e2.strokeStyle = t2.colorBarStroke, e2.stroke()), "progress" !== i2 && t2.colorBar ? (e2.fillStyle = t2.colorBarEnd ? We.linearGradient(e2, t2.colorBar, t2.colorBarEnd, h2, s2, s2 ? p2 : b2) : t2.colorBar, e2.fill()) : "progress" === i2 && t2.colorBarProgress && (e2.fillStyle = t2.colorBarProgressEnd ? We.linearGradient(e2, t2.colorBarProgress, t2.colorBarProgressEnd, w2, s2, s2 ? p2 : b2) : t2.colorBarProgress, e2.fill()), e2.closePath(), t2.barBeginCircle && (e2.barDimensions.radius += u2), e2.barDimensions.barWidth += u2, e2.barDimensions.barLength += u2;
      }
      function U(e2, t2, i2, r2, o2, n2) {
        Y(e2, t2, "", i2, r2, o2, n2);
      }
      function q(e2, t2) {
        return t2.needleSide !== e2 || t2.tickSide !== e2 || t2.numberSide !== e2;
      }
      function H(e2, t2, i2, r2, o2, n2) {
        t2.barProgress && Y(e2, t2, "progress", i2, r2, o2, n2);
      }
      function J(e2, t2) {
        var i2 = e2.barDimensions, r2 = i2.isVertical, o2 = i2.width, n2 = i2.length, a2 = i2.barWidth, l2 = i2.barOffset, s2 = i2.barMargin, d2 = i2.X, c2 = i2.Y, h2 = i2.ticksLength, u2 = i2.ticksPadding, f2 = o2 * (parseFloat(t2.highlightsWidth) || 0) / 100;
        if (t2.highlights && f2) {
          var m2 = "right" !== t2.tickSide, v2 = "left" !== t2.tickSide, g2 = 0, b2 = t2.highlights.length, p2 = (o2 - a2) / 2, w2 = t2.maxValue - t2.minValue, k2 = ye(r2 ? d2 + p2 : d2 + s2 + l2), y2 = f2, x2 = r2 ? c2 + n2 - s2 - l2 : c2 + p2, T2 = ye((t2.ticksWidth / 100 + u2) * o2) + (f2 - t2.ticksWidth / 100 * o2), S2 = ye(a2 + u2 * o2);
          for (e2.save(); g2 < b2; g2++) {
            var V2 = t2.highlights[g2], W2 = h2 * xe(t2.minValue - V2.from) / w2, O2 = h2 * xe((V2.to - V2.from) / w2);
            e2.beginPath(), e2.fillStyle = V2.color, r2 ? (m2 && e2.rect(k2 - T2, x2 - W2, y2, -O2), v2 && e2.rect(k2 + S2, x2 - W2, y2, -O2)) : (m2 && e2.rect(k2 + W2, x2 - T2, O2, y2), v2 && e2.rect(k2 + W2, x2 + S2, O2, y2)), e2.fill(), e2.closePath();
          }
        }
      }
      function Z(e2, t2, i2, r2, o2) {
        e2.beginPath(), e2.moveTo(t2, i2), e2.lineTo(r2, o2), e2.stroke(), e2.closePath(), e2.save();
      }
      function $(e2, t2, i2, r2, o2, n2, a2, l2, s2) {
        var d2 = e2.barDimensions, c2 = d2.isVertical, h2 = d2.length, u2 = d2.barWidth, f2 = d2.barOffset, m2 = d2.barMargin, v2 = d2.pixelRatio, g2 = d2.width, b2 = d2.X, p2 = d2.Y, w2 = d2.ticksLength, k2 = d2.ticksPadding, y2 = (g2 - u2) / 2, x2 = void 0, T2 = void 0, S2 = 0, V2 = i2.length, W2 = void 0, O2 = s2 * g2, A2 = y2 - k2 * g2, P2 = y2 + u2 + O2 + k2 * g2, M2 = t2 instanceof Array ? t2 : new Array(i2.length).fill(t2);
        e2.lineWidth = l2 * v2, e2.save();
        for (var B2 = w2 / (o2 - r2); S2 < V2; S2++) W2 = i2[S2], e2.strokeStyle = M2[S2], c2 ? (T2 = p2 + h2 - m2 - f2 + (r2 - W2) * B2, n2 && (x2 = b2 + A2, Z(e2, x2, T2, ye(x2 - O2), T2)), a2 && (x2 = b2 + P2, Z(e2, x2, T2, ye(x2 - O2), T2))) : (x2 = b2 + m2 + f2 - (r2 - W2) * B2, n2 && (T2 = p2 + A2, Z(e2, x2, T2, x2, ye(T2 - O2))), a2 && (T2 = p2 + P2, Z(e2, x2, ye(T2), x2, T2 - O2)));
      }
      function K(e2, t2) {
        var i2 = We.prepareTicks(t2), r2 = de(i2, 2), o2 = r2[0], n2 = r2[1], a2 = 2, l2 = (t2.maxValue - t2.minValue) / (t2.majorTicks.length - 1), s2 = t2.colorMajorTicks instanceof Array ? t2.colorMajorTicks : new Array(t2.majorTicks.length).fill(t2.colorStrokeTicks || t2.colorMajorTicks);
        if ($(e2, s2, t2.exactTicks ? t2.majorTicks : t2.majorTicks.map(function(e3, i3) {
          return t2.minValue + l2 * i3;
        }), t2.minValue, t2.maxValue, o2, n2, a2, t2.ticksWidth / 100), t2.strokeTicks) {
          var d2 = e2.barDimensions, c2 = d2.isVertical, h2 = d2.length, u2 = d2.width, f2 = d2.barWidth, m2 = d2.barMargin, v2 = d2.barOffset, g2 = d2.X, b2 = d2.Y, p2 = d2.ticksLength, w2 = d2.pixelRatio, k2 = d2.ticksPadding, y2 = (u2 - f2) / 2 + f2 + k2 * u2, x2 = (u2 - f2) / 2 - k2 * u2, T2 = void 0, S2 = void 0, V2 = void 0, W2 = void 0;
          e2.strokeStyle = t2.colorStrokeTicks || s2[0], a2 *= w2, c2 ? (S2 = b2 + h2 - m2 - v2 + a2 / 2, W2 = S2 - p2 - a2, o2 && (V2 = T2 = ye(g2 + x2), Q(e2, T2, S2, V2, W2)), n2 && (V2 = T2 = ye(g2 + y2), Q(e2, T2, S2, V2, W2))) : (T2 = g2 + m2 + v2 - a2 / 2, V2 = T2 + p2 + a2, o2 && (W2 = S2 = ye(b2 + x2), Q(e2, T2, S2, V2, W2)), n2 && (W2 = S2 = ye(b2 + y2), Q(e2, T2, S2, V2, W2)));
        }
      }
      function Q(e2, t2, i2, r2, o2) {
        e2.beginPath(), e2.moveTo(t2, i2), e2.lineTo(r2, o2), e2.stroke(), e2.closePath();
      }
      function ee(e2, t2) {
        var i2 = We.prepareTicks(t2), r2 = de(i2, 2), o2 = r2[0], n2 = r2[1], a2 = [], l2 = t2.minValue, s2 = Math.abs(t2.minorTicks) || 0, d2 = s2 ? (t2.maxValue - t2.minValue) / (s2 * (t2.majorTicks.length - 1)) : 0;
        if (s2) if (t2.exactTicks) for (var c2 = Se.mod(t2.majorTicks[0], s2) || 0; l2 < t2.maxValue; l2 += s2) c2 + l2 < t2.maxValue && a2.push(c2 + l2);
        else for (; l2 < t2.maxValue; l2 += d2) a2.push(l2);
        $(e2, t2.colorMinorTicks || t2.colorStrokeTicks, a2, t2.minValue, t2.maxValue, o2, n2, 1, t2.ticksWidthMinor / 100);
      }
      function te(e2, t2) {
        var i2 = e2.barDimensions, r2 = i2.isVertical, o2 = i2.length, n2 = i2.width, a2 = i2.barWidth, l2 = i2.barMargin, s2 = i2.barOffset, d2 = i2.X, c2 = i2.Y, h2 = i2.ticksLength, u2 = i2.ticksPadding, f2 = t2.maxValue - t2.minValue, m2 = f2 / (t2.majorTicks.length - 1), v2 = t2.exactTicks ? t2.majorTicks : t2.majorTicks.map(function(e3, i3) {
          return t2.minValue + m2 * i3;
        }), g2 = v2.length, b2 = "right" !== t2.numberSide, p2 = "left" !== t2.numberSide, w2 = t2.fontNumbersSize * n2 / 200, k2 = 0, y2 = (t2.ticksWidth / 100 + 2 * u2) * n2, x2 = (n2 - a2) / 2 - y2, T2 = (n2 - a2) / 2 + a2 + y2, S2 = void 0, V2 = void 0, W2 = void 0, O2 = void 0, A2 = t2.colorNumbers instanceof Array ? t2.colorNumbers : new Array(g2).fill(t2.colorNumbers), P2 = t2.numbersMargin / 100 * n2;
        for (e2.font = We.font(t2, "Numbers", n2 / 200), e2.lineWidth = 0, e2.textAlign = "center"; k2 < g2; k2++) e2.fillStyle = A2[k2], O2 = t2.majorTicks[k2], W2 = t2.exactTicks ? h2 * ((v2[k2] - t2.minValue) / f2) : k2 * h2 / (g2 - 1), r2 ? (V2 = c2 + o2 - l2 - s2 - W2 + w2 / 3, b2 && (e2.textAlign = "right", e2.fillText(O2, d2 + x2 - P2, V2)), p2 && (e2.textAlign = "left", e2.fillText(O2, d2 + T2 + P2, V2))) : (e2.measureText(O2).width, S2 = d2 + l2 + s2 + W2, b2 && e2.fillText(O2, S2, c2 + x2 - P2), p2 && e2.fillText(O2, S2, c2 + T2 + w2 + P2));
      }
      function ie(e2, t2) {
        if (t2.title) {
          var i2 = e2.barDimensions, r2 = i2.isVertical, o2 = i2.width, n2 = i2.length, a2 = i2.baseX, l2 = i2.baseY, s2 = i2.titleMargin, d2 = t2.fontTitleSize * o2 / 200, c2 = ye(a2 + (r2 ? o2 : n2) / 2), h2 = ye(l2 + s2 / 2 - (r2 ? d2 : d2 / 2) - 0.025 * (r2 ? n2 : o2));
          e2.save(), e2.textAlign = "center", e2.fillStyle = t2.colorTitle, e2.font = We.font(t2, "Title", o2 / 200), e2.lineWidth = 0, e2.fillText(t2.title, c2, h2, r2 ? o2 : n2);
        }
      }
      function re(e2, t2) {
        if (t2.units) {
          var i2 = e2.barDimensions, r2 = i2.isVertical, o2 = i2.width, n2 = i2.length, a2 = i2.baseX, l2 = i2.baseY, s2 = i2.unitsMargin, d2 = t2.fontUnitsSize * o2 / 200, c2 = ye(a2 + (r2 ? o2 : n2) / 2), h2 = ye(l2 + (r2 ? n2 : o2) + s2 / 2 - d2 / 2);
          e2.save(), e2.textAlign = "center", e2.fillStyle = t2.colorUnits, e2.font = We.font(t2, "Units", o2 / 200), e2.lineWidth = 0, e2.fillText(We.formatContext(t2, t2.units), c2, h2, r2 ? o2 : n2);
        }
      }
      function oe(e2, t2) {
        if (t2.needle) {
          var i2 = e2.barDimensions, r2 = i2.isVertical, o2 = i2.width, n2 = i2.length, a2 = i2.barWidth, l2 = i2.barOffset, s2 = i2.barMargin, d2 = i2.ticksLength, c2 = i2.X, h2 = i2.Y, u2 = i2.ticksPadding, f2 = "right" !== t2.needleSide, m2 = "left" !== t2.needleSide, v2 = d2 * (We.normalizedValue(t2).indented - t2.minValue) / (t2.maxValue - t2.minValue), g2 = (t2.ticksWidth / 100 + u2) * o2, b2 = a2 / 2 + g2, p2 = b2 * (t2.needleEnd / 100), w2 = void 0, k2 = void 0, y2 = void 0, x2 = void 0, T2 = "arrow" === t2.needleType.toLowerCase() ? le : ae, S2 = (o2 - a2) / 2, V2 = b2 * (t2.needleStart / 100), W2 = S2 - g2 - V2, O2 = S2 + a2 + g2 + V2;
          e2.save(), We.drawNeedleShadow(e2, t2), r2 ? (y2 = ye(h2 + n2 - s2 - l2 - v2), f2 && (w2 = ye(c2 + W2), k2 = w2 + p2, T2(e2, t2, w2, y2, k2, y2, p2)), m2 && (w2 = ye(c2 + O2), k2 = w2 - p2, T2(e2, t2, w2, y2, k2, y2, p2, true))) : (w2 = ye(c2 + s2 + l2 + v2), f2 && (y2 = ye(h2 + W2), x2 = y2 + p2, T2(e2, t2, w2, y2, w2, x2, p2)), m2 && (y2 = ye(h2 + O2), x2 = y2 - p2, T2(e2, t2, w2, y2, w2, x2, p2, true))), e2.restore();
        }
      }
      function ne(e2, t2, i2, r2) {
        return t2.colorNeedleEnd ? We.linearGradient(e2, r2 ? t2.colorNeedleEnd : t2.colorNeedle, r2 ? t2.colorNeedle : t2.colorNeedleEnd, i2, !e2.barDimensions.isVertical) : t2.colorNeedle;
      }
      function ae(e2, t2, i2, r2, o2, n2, a2, l2) {
        e2.lineWidth = t2.needleWidth, e2.strokeStyle = ne(e2, t2, a2, l2), e2.beginPath(), e2.moveTo(i2, r2), e2.lineTo(o2, n2), e2.stroke(), e2.closePath();
      }
      function le(e2, t2, i2, r2, o2, n2, a2, l2) {
        var s2 = ye(0.4 * a2), d2 = a2 - s2, c2 = i2 === o2, h2 = t2.needleWidth / 2;
        e2.fillStyle = ne(e2, t2, a2, l2), e2.beginPath(), c2 ? (r2 > n2 && (d2 *= -1), e2.moveTo(i2 - h2, r2), e2.lineTo(i2 + h2, r2), e2.lineTo(i2 + h2, r2 + d2), e2.lineTo(i2, n2), e2.lineTo(i2 - h2, r2 + d2), e2.lineTo(i2 - h2, r2)) : (i2 > o2 && (d2 *= -1), e2.moveTo(i2, r2 - h2), e2.lineTo(i2, r2 + h2), e2.lineTo(i2 + d2, r2 + h2), e2.lineTo(o2, r2), e2.lineTo(i2 + d2, r2 - h2), e2.lineTo(i2, r2 - h2)), e2.fill(), e2.closePath();
      }
      function se(e2, t2, i2, r2, o2, n2, a2) {
        var l2 = (parseFloat(t2.fontValueSize) || 0) * n2 / 200, s2 = (0.11 * a2 - l2) / 2;
        e2.barDimensions.isVertical && We.drawValueBox(e2, t2, i2, r2 + n2 / 2, o2 + a2 - l2 - s2, n2);
      }
      var de = /* @__PURE__ */ function() {
        function e2(e3, t2) {
          var i2 = [], r2 = true, o2 = false, n2 = void 0;
          try {
            for (var a2, l2 = e3[Symbol.iterator](); !(r2 = (a2 = l2.next()).done) && (i2.push(a2.value), !t2 || i2.length !== t2); r2 = true) ;
          } catch (e4) {
            o2 = true, n2 = e4;
          } finally {
            try {
              !r2 && l2.return && l2.return();
            } finally {
              if (o2) throw n2;
            }
          }
          return i2;
        }
        return function(t2, i2) {
          if (Array.isArray(t2)) return t2;
          if (Symbol.iterator in Object(t2)) return e2(t2, i2);
          throw new TypeError("Invalid attempt to destructure non-iterable instance");
        };
      }(), ce = function e2(t2, i2, r2) {
        null === t2 && (t2 = Function.prototype);
        var o2 = Object.getOwnPropertyDescriptor(t2, i2);
        if (void 0 === o2) {
          var n2 = Object.getPrototypeOf(t2);
          return null === n2 ? void 0 : e2(n2, i2, r2);
        }
        if ("value" in o2) return o2.value;
        var a2 = o2.get;
        if (void 0 !== a2) return a2.call(r2);
      }, he = function e2(t2, i2, r2, o2) {
        var n2 = Object.getOwnPropertyDescriptor(t2, i2);
        if (void 0 === n2) {
          var a2 = Object.getPrototypeOf(t2);
          null !== a2 && e2(a2, i2, r2, o2);
        } else if ("value" in n2 && n2.writable) n2.value = r2;
        else {
          var l2 = n2.set;
          void 0 !== l2 && l2.call(o2, r2);
        }
        return r2;
      }, ue = /* @__PURE__ */ function() {
        function e2(e3, t2) {
          for (var i2 = 0; i2 < t2.length; i2++) {
            var r2 = t2[i2];
            r2.enumerable = r2.enumerable || false, r2.configurable = true, "value" in r2 && (r2.writable = true), Object.defineProperty(e3, r2.key, r2);
          }
        }
        return function(t2, i2, r2) {
          return i2 && e2(t2.prototype, i2), r2 && e2(t2, r2), t2;
        };
      }();
      Object.assign || Object.defineProperty(Object, "assign", { enumerable: false, configurable: true, writable: true, value: function(e2, t2) {
        if (void 0 === e2 || null === e2) throw new TypeError("Cannot convert first argument to object");
        for (var i2 = Object(e2), r2 = 1; r2 < arguments.length; r2++) {
          var o2 = arguments[r2];
          if (void 0 !== o2 && null !== o2) for (var n2 = Object.keys(Object(o2)), a2 = 0, l2 = n2.length; a2 < l2; a2++) {
            var s2 = n2[a2], d2 = Object.getOwnPropertyDescriptor(o2, s2);
            void 0 !== d2 && d2.enumerable && (i2[s2] = o2[s2]);
          }
        }
        return i2;
      } }), Array.prototype.indexOf || Object.defineProperty(Array.prototype, "indexOf", { value: function(e2, t2) {
        var i2;
        if (null === this) throw new TypeError('"this" is null or not defined');
        var r2 = Object(this), o2 = r2.length >>> 0;
        if (0 === o2) return -1;
        var n2 = +t2 || 0;
        if (Math.abs(n2) === 1 / 0 && (n2 = 0), n2 >= o2) return -1;
        for (i2 = Math.max(n2 >= 0 ? n2 : o2 - Math.abs(n2), 0); i2 < o2; ) {
          if (i2 in r2 && r2[i2] === e2) return i2;
          i2++;
        }
        return -1;
      } }), Array.prototype.fill || Object.defineProperty(Array.prototype, "fill", { value: function(e2) {
        if (null === this) throw new TypeError("this is null or not defined");
        for (var t2 = Object(this), i2 = t2.length >>> 0, r2 = arguments[1], o2 = r2 >> 0, n2 = o2 < 0 ? Math.max(i2 + o2, 0) : Math.min(o2, i2), a2 = arguments[2], l2 = void 0 === a2 ? i2 : a2 >> 0, s2 = l2 < 0 ? Math.max(i2 + l2, 0) : Math.min(l2, i2); n2 < s2; ) t2[n2] = e2, n2++;
        return t2;
      } }), "undefined" == typeof window && (window = "undefined" == typeof global ? {} : global);
      var fe = function() {
        function e2() {
          o(this, e2), this._events = {}, this.addListener = this.on, this.removeListener = this.off;
        }
        return ue(e2, [{ key: "emit", value: function(e3) {
          if (this._events[e3]) {
            for (var t2 = 0, i2 = this._events[e3].length, r2 = arguments.length, o2 = Array(r2 > 1 ? r2 - 1 : 0), n2 = 1; n2 < r2; n2++) o2[n2 - 1] = arguments[n2];
            for (; t2 < i2; t2++) this._events[e3][t2] && this._events[e3][t2].apply(this, o2);
          }
        } }, { key: "once", value: function(e3) {
          for (var t2 = arguments.length, i2 = Array(t2 > 1 ? t2 - 1 : 0), r2 = 1; r2 < t2; r2++) i2[r2 - 1] = arguments[r2];
          for (var o2 = 0, n2 = i2.length, a2 = this; o2 < n2; o2++) !function() {
            var t3 = i2[o2], r3 = function i3() {
              a2.off(e3, i3), t3.apply(a2, arguments);
            };
            i2[o2] = r3;
          }();
          this.on.apply(this, [e3].concat(i2));
        } }, { key: "on", value: function(e3) {
          this._events[e3] || (this._events[e3] = []);
          for (var t2 = 0, i2 = arguments.length <= 1 ? 0 : arguments.length - 1; t2 < i2; t2++) this._events[e3].push(arguments.length <= t2 + 1 ? void 0 : arguments[t2 + 1]);
        } }, { key: "off", value: function(e3) {
          if (this._events[e3]) for (var t2 = 0, i2 = arguments.length <= 1 ? 0 : arguments.length - 1; t2 < i2; t2++) for (var r2 = arguments.length <= t2 + 1 ? void 0 : arguments[t2 + 1], o2 = void 0; ~(o2 = this._events[e3].indexOf(r2)); ) this._events[e3].splice(o2, 1);
        } }, { key: "removeAllListeners", value: function(e3) {
          delete this._events[e3];
        } }, { key: "listeners", get: function() {
          return this._events;
        } }]), e2;
      }(), me = n("requestAnimationFrame") || function(e2) {
        return setTimeout(function() {
          return e2((/* @__PURE__ */ new Date()).getTime());
        }, 1e3 / 60);
      }, ve = { linear: function(e2) {
        return e2;
      }, quad: function(e2) {
        return Math.pow(e2, 2);
      }, dequad: function(e2) {
        return 1 - ve.quad(1 - e2);
      }, quint: function(e2) {
        return Math.pow(e2, 5);
      }, dequint: function(e2) {
        return 1 - Math.pow(1 - e2, 5);
      }, cycle: function(e2) {
        return 1 - Math.sin(Math.acos(e2));
      }, decycle: function(e2) {
        return Math.sin(Math.acos(1 - e2));
      }, bounce: function(e2) {
        return 1 - ve.debounce(1 - e2);
      }, debounce: function(e2) {
        for (var t2 = 0, i2 = 1; 1; t2 += i2, i2 /= 2) if (e2 >= (7 - 4 * t2) / 11) return -Math.pow((11 - 6 * t2 - 11 * e2) / 4, 2) + Math.pow(i2, 2);
      }, elastic: function(e2) {
        return 1 - ve.delastic(1 - e2);
      }, delastic: function(e2) {
        return Math.pow(2, 10 * (e2 - 1)) * Math.cos(20 * Math.PI * 1.5 / 3 * e2);
      } }, ge = function() {
        function e2() {
          var t2 = arguments.length > 0 && void 0 !== arguments[0] ? arguments[0] : "linear", i2 = arguments.length > 1 && void 0 !== arguments[1] ? arguments[1] : 250, r2 = arguments.length > 2 && void 0 !== arguments[2] ? arguments[2] : function() {
          }, n2 = arguments.length > 3 && void 0 !== arguments[3] ? arguments[3] : function() {
          };
          if (o(this, e2), this.duration = i2, this.rule = t2, this.draw = r2, this.end = n2, "function" != typeof this.draw) throw new TypeError("Invalid animation draw callback:", r2);
          if ("function" != typeof this.end) throw new TypeError("Invalid animation end callback:", n2);
        }
        return ue(e2, [{ key: "animate", value: function(e3, t2) {
          var i2 = this;
          this.frame && this.cancel();
          var r2 = window.performance && window.performance.now ? window.performance.now() : n("animationStartTime") || Date.now();
          e3 = e3 || this.draw, t2 = t2 || this.end, this.draw = e3, this.end = t2, this.frame = me(function(o2) {
            return a(o2, e3, r2, ve[i2.rule] || i2.rule, i2.duration, t2, i2);
          });
        } }, { key: "cancel", value: function() {
          if (this.frame) {
            (n("cancelAnimationFrame") || function(e3) {
            })(this.frame), this.frame = null;
          }
        } }, { key: "destroy", value: function() {
          this.cancel(), this.draw = null, this.end = null;
        } }]), e2;
      }();
      ge.rules = ve;
      var be = function() {
        function t2(i2, r2, n2) {
          o(this, t2), this.options = i2, this.element = r2.toLowerCase(), this.type = t2.toDashed(n2), this.Type = e[n2], this.mutationsObserved = false, this.isObservable = !!window.MutationObserver, window.GAUGES_NO_AUTO_INIT || t2.domReady(this.traverse.bind(this));
        }
        return ue(t2, [{ key: "isValidNode", value: function(e2) {
          return !(!e2.tagName || e2.tagName.toLowerCase() !== this.element || e2.getAttribute("data-type") !== this.type);
        } }, { key: "traverse", value: function() {
          for (var e2 = document.getElementsByTagName(this.element), t3 = 0, i2 = e2.length; t3 < i2; t3++) this.process(e2[t3]);
          this.isObservable && !this.mutationsObserved && (new MutationObserver(this.observe.bind(this)).observe(document.body, { childList: true, subtree: true, attributes: true, characterData: true, attributeOldValue: true, characterDataOldValue: true }), this.mutationsObserved = true);
        } }, { key: "observe", value: function(e2) {
          for (var t3 = 0, i2 = e2.length; t3 < i2; t3++) {
            var r2 = e2[t3];
            if ("attributes" === r2.type && "data-type" === r2.attributeName && this.isValidNode(r2.target) && r2.oldValue !== this.type) setTimeout(this.process.bind(this, r2.target));
            else if (r2.addedNodes && r2.addedNodes.length) for (var o2 = 0, n2 = r2.addedNodes.length; o2 < n2; o2++) setTimeout(this.process.bind(this, r2.addedNodes[o2]));
          }
        } }, { key: "process", value: function(e2) {
          var i2 = this;
          if (!this.isValidNode(e2)) return null;
          var r2 = void 0, o2 = JSON.parse(JSON.stringify(this.options)), n2 = null;
          for (r2 in o2) if (o2.hasOwnProperty(r2)) {
            var a2 = t2.toAttributeName(r2), l2 = t2.parse(e2.getAttribute(a2));
            null !== l2 && void 0 !== l2 && (o2[r2] = l2);
          }
          return o2.renderTo = e2, n2 = new this.Type(o2), n2.draw && n2.draw(), this.isObservable ? (n2.observer = new MutationObserver(function(r3) {
            r3.forEach(function(r4) {
              if ("attributes" === r4.type) {
                var o3 = r4.attributeName.toLowerCase(), a3 = e2.getAttribute(o3).toLowerCase();
                if ("data-type" === o3 && a3 && a3 !== i2.type) n2.observer.disconnect(), delete n2.observer, n2.destroy && n2.destroy();
                else if ("data-" === o3.substr(0, 5)) {
                  var l3 = o3.substr(5).split("-").map(function(e3, t3) {
                    return t3 ? e3.charAt(0).toUpperCase() + e3.substr(1) : e3;
                  }).join(""), s2 = {};
                  s2[l3] = t2.parse(e2.getAttribute(r4.attributeName)), "value" === l3 ? n2 && (n2.value = s2[l3]) : n2.update && n2.update(s2);
                }
              }
            });
          }), n2.observer.observe(e2, { attributes: true }), n2) : n2;
        } }], [{ key: "parse", value: function(e2) {
          if ("true" === e2) return true;
          if ("false" === e2) return false;
          if ("undefined" !== e2) {
            if ("null" === e2) return null;
            if (/^[-+#.\w\d\s]+(?:,[-+#.\w\d\s]*)+$/.test(e2)) return e2.split(",");
            try {
              return JSON.parse(e2);
            } catch (e3) {
            }
            return e2;
          }
        } }, { key: "toDashed", value: function(e2) {
          for (var t3 = e2.split(/(?=[A-Z])/), i2 = 1, r2 = t3.length, o2 = t3[0].toLowerCase(); i2 < r2; i2++) o2 += "-" + t3[i2].toLowerCase();
          return o2;
        } }, { key: "toCamelCase", value: function(e2) {
          for (var t3 = !(arguments.length > 1 && void 0 !== arguments[1]) || arguments[1], i2 = e2.split(/-/), r2 = 0, o2 = i2.length, n2 = ""; r2 < o2; r2++) n2 += r2 || t3 ? i2[r2][0].toUpperCase() + i2[r2].substr(1).toLowerCase() : i2[r2].toLowerCase();
          return n2;
        } }, { key: "toAttributeName", value: function(e2) {
          return "data-" + t2.toDashed(e2);
        } }, { key: "domReady", value: function(e2) {
          if (/comp|inter|loaded/.test((window.document || {}).readyState + "")) return e2();
          window.addEventListener ? window.addEventListener("DOMContentLoaded", e2, false) : window.attachEvent && window.attachEvent("onload", e2);
        } }]), t2;
      }(), pe = function() {
        function e2(t2, i2, r2) {
          o(this, e2), e2.collection.push(this), this.width = i2 || 0, this.height = r2 || 0, this.element = t2, this.init();
        }
        return ue(e2, [{ key: "init", value: function() {
          var t2 = e2.pixelRatio;
          this.element.width = this.width * t2, this.element.height = this.height * t2, this.element.style.width = this.width + "px", this.element.style.height = this.height + "px", this.elementClone = this.element.cloneNode(true), this.context = this.element.getContext("2d"), this.contextClone = this.elementClone.getContext("2d"), this.drawWidth = this.element.width, this.drawHeight = this.element.height, this.drawX = this.drawWidth / 2, this.drawY = this.drawHeight / 2, this.minSide = this.drawX < this.drawY ? this.drawX : this.drawY, this.elementClone.initialized = false, this.contextClone.translate(this.drawX, this.drawY), this.contextClone.save(), this.context.translate(this.drawX, this.drawY), this.context.save(), this.context.max = this.contextClone.max = this.minSide, this.context.maxRadius = this.contextClone.maxRadius = null;
        } }, { key: "destroy", value: function() {
          var t2 = e2.collection.indexOf(this);
          ~t2 && e2.collection.splice(t2, 1), this.context.clearRect(-this.drawX, -this.drawY, this.drawWidth, this.drawHeight), this.context.max = null, delete this.context.max, this.context.maxRadius = null, delete this.context.maxRadius, this.context = null, this.contextClone = null, this.elementClone = null, this.element = null, this.onRedraw = null;
        } }, { key: "commit", value: function() {
          var t2 = e2.pixelRatio;
          return 1 !== t2 && (this.contextClone.scale(t2, t2), this.contextClone.save()), this;
        } }, { key: "redraw", value: function() {
          return this.init(), this.onRedraw && this.onRedraw(), this;
        } }], [{ key: "redraw", value: function() {
          for (var t2 = 0, i2 = e2.collection.length; t2 < i2; t2++) e2.collection[t2].redraw();
        } }, { key: "pixelRatio", get: function() {
          return window.devicePixelRatio || 1;
        } }]), e2;
      }();
      pe.collection = [], window.matchMedia && window.matchMedia("screen and (min-resolution: 2dppx)").addListener(pe.redraw);
      var we = { renderTo: null, width: 0, height: 0, minValue: 0, maxValue: 100, value: 0, units: false, exactTicks: false, majorTicks: [0, 20, 40, 60, 80, 100], minorTicks: 10, strokeTicks: true, animatedValue: false, animateOnInit: false, title: false, borders: true, numbersMargin: 1, listeners: null, valueInt: 3, valueDec: 2, majorTicksInt: 1, majorTicksDec: 0, animation: true, animationDuration: 500, animationRule: "cycle", colorPlate: "#fff", colorPlateEnd: "", colorMajorTicks: "#444", colorMinorTicks: "#666", colorStrokeTicks: "", colorTitle: "#888", colorUnits: "#888", colorNumbers: "#444", colorNeedle: "rgba(240,128,128,1)", colorNeedleEnd: "rgba(255,160,122,.9)", colorValueText: "#444", colorValueTextShadow: "rgba(0,0,0,0.3)", colorBorderShadow: "rgba(0,0,0,0.5)", colorBorderOuter: "#ddd", colorBorderOuterEnd: "#aaa", colorBorderMiddle: "#eee", colorBorderMiddleEnd: "#f0f0f0", colorBorderInner: "#fafafa", colorBorderInnerEnd: "#ccc", colorValueBoxRect: "#888", colorValueBoxRectEnd: "#666", colorValueBoxBackground: "#babab2", colorValueBoxShadow: "rgba(0,0,0,1)", colorNeedleShadowUp: "rgba(2,255,255,0.2)", colorNeedleShadowDown: "rgba(188,143,143,0.45)", colorBarStroke: "#222", colorBar: "#ccc", colorBarProgress: "#888", colorBarShadow: "#000", fontNumbers: "Arial", fontTitle: "Arial", fontUnits: "Arial", fontValue: "Arial", fontNumbersSize: 20, fontTitleSize: 24, fontUnitsSize: 22, fontValueSize: 26, fontNumbersStyle: "normal", fontTitleStyle: "normal", fontUnitsStyle: "normal", fontValueStyle: "normal", fontNumbersWeight: "normal", fontTitleWeight: "normal", fontUnitsWeight: "normal", fontValueWeight: "normal", needle: true, needleShadow: true, needleType: "arrow", needleStart: 5, needleEnd: 85, needleWidth: 4, borderOuterWidth: 3, borderMiddleWidth: 3, borderInnerWidth: 3, borderShadowWidth: 3, valueBox: true, valueBoxStroke: 5, valueBoxWidth: 0, valueText: "", valueTextShadow: true, valueBoxBorderRadius: 2.5, highlights: [{ from: 20, to: 60, color: "#eee" }, { from: 60, to: 80, color: "#ccc" }, { from: 80, to: 100, color: "#999" }], highlightsWidth: 15, highlightsLineCap: "butt", barWidth: 20, barStrokeWidth: 0, barProgress: true, barShadow: 0 };
      l.prototype = Object.create(Array.prototype), l.prototype.constructor = l, l.prototype.get = function(e2) {
        if ("string" == typeof e2) for (var t2 = 0, i2 = this.length; t2 < i2; t2++) {
          var r2 = this[t2].options.renderTo.tagName ? this[t2].options.renderTo : document.getElementById(this[t2].options.renderTo || "");
          if (r2.getAttribute("id") === e2) return this[t2];
        }
        else if ("number" == typeof e2) return this[e2];
        return null;
      };
      var ke = "2.1.7", ye = Math.round, xe = Math.abs, Te = new l();
      Te.version = ke;
      var Se = function(t2) {
        function n2(t3) {
          o(this, n2);
          var r2 = i(this, (n2.__proto__ || Object.getPrototypeOf(n2)).call(this)), a2 = r2.constructor.name;
          if ("BaseGauge" === a2) throw new TypeError("Attempt to instantiate abstract class!");
          if (Te.push(r2), t3.listeners && Object.keys(t3.listeners).forEach(function(e2) {
            (t3.listeners[e2] instanceof Array ? t3.listeners[e2] : [t3.listeners[e2]]).forEach(function(t4) {
              r2.on(e2, t4);
            });
          }), r2.version = ke, r2.type = e[a2] || n2, r2.initialized = false, t3.minValue = parseFloat(t3.minValue), t3.maxValue = parseFloat(t3.maxValue), t3.value = parseFloat(t3.value) || 0, t3.borders || (t3.borderInnerWidth = t3.borderMiddleWidth = t3.borderOuterWidth = 0), !t3.renderTo) throw TypeError("Canvas element was not specified when creating the Gauge object!");
          var l2 = t3.renderTo.tagName ? t3.renderTo : document.getElementById(t3.renderTo);
          if (!(l2 instanceof HTMLCanvasElement)) throw TypeError("Given gauge canvas element is invalid!");
          return t3.width = parseFloat(t3.width) || 0, t3.height = parseFloat(t3.height) || 0, t3.width && t3.height || (t3.width || (t3.width = l2.parentNode ? l2.parentNode.offsetWidth : l2.offsetWidth), t3.height || (t3.height = l2.parentNode ? l2.parentNode.offsetHeight : l2.offsetHeight)), r2.options = t3 || {}, r2.options.animateOnInit && (r2._value = r2.options.value, r2.options.value = r2.options.minValue), r2.canvas = new pe(l2, t3.width, t3.height), r2.canvas.onRedraw = r2.draw.bind(r2), r2.animation = new ge(t3.animationRule, t3.animationDuration), r2;
        }
        return r(n2, t2), ue(n2, [{ key: "update", value: function(e2) {
          return Object.assign(this.options, this.type.configure(e2 || {})), this.canvas.width = this.options.width, this.canvas.height = this.options.height, this.animation.rule = this.options.animationRule, this.animation.duration = this.options.animationDuration, this.canvas.redraw(), this;
        } }, { key: "destroy", value: function() {
          var e2 = Te.indexOf(this);
          ~e2 && Te.splice(e2, 1), this.canvas.destroy(), this.canvas = null, this.animation.destroy(), this.animation = null, this.emit("destroy");
        } }, { key: "draw", value: function() {
          return this.options.animateOnInit && !this.initialized && (this.value = this._value, this.initialized = true, this.emit("init")), this.emit("render"), this;
        } }, { key: "value", set: function(e2) {
          var t3 = this;
          e2 = n2.ensureValue(e2, this.options.minValue);
          var i2 = this.options.value;
          if (e2 !== i2) if (this.options.animation) {
            if (this.animation.frame && (this.options.value = this._value, this._value === e2)) return this.animation.cancel(), void delete this._value;
            void 0 === this._value && (this._value = e2), this.emit("animationStart"), this.animation.animate(function(r2) {
              var o2 = i2 + (e2 - i2) * r2;
              t3.options.animatedValue && t3.emit("value", o2, t3.value), t3.options.value = o2, t3.draw(), t3.emit("animate", r2, t3.options.value);
            }, function() {
              void 0 !== t3._value && (t3.emit("value", t3._value, t3.value), t3.options.value = t3._value, delete t3._value), t3.draw(), t3.emit("animationEnd");
            });
          } else this.emit("value", e2, this.value), this.options.value = e2, this.draw();
        }, get: function() {
          return void 0 === this._value ? this.options.value : this._value;
        } }], [{ key: "configure", value: function(e2) {
          return e2;
        } }, { key: "initialize", value: function(e2, t3) {
          return new be(t3, "canvas", e2);
        } }, { key: "fromElement", value: function(e2) {
          var t3 = be.toCamelCase(e2.getAttribute("data-type")), i2 = e2.attributes, r2 = 0, o2 = i2.length, n3 = {};
          if (t3) {
            for (/Gauge$/.test(t3) || (t3 += "Gauge"); r2 < o2; r2++) n3[be.toCamelCase(i2[r2].name.replace(/^data-/, ""), false)] = be.parse(i2[r2].value);
            new be(n3, e2.tagName, t3).process(e2);
          }
        } }, { key: "ensureValue", value: function(e2) {
          var t3 = arguments.length > 1 && void 0 !== arguments[1] ? arguments[1] : 0;
          return e2 = parseFloat(e2), !isNaN(e2) && isFinite(e2) || (e2 = parseFloat(t3) || 0), e2;
        } }, { key: "mod", value: function(e2, t3) {
          return (e2 % t3 + t3) % t3;
        } }, { key: "version", get: function() {
          return ke;
        } }]), n2;
      }(fe);
      void 0 !== e && (e.BaseGauge = Se, e.gauges = (window.document || {}).gauges = Te);
      var Ve = /{([_a-zA-Z]+[_a-zA-Z0-9]*)}/g, We = { roundRect: h, padValue: u, formatMajorTickNumber: f, radians: m, radialPoint: v, linearGradient: g, drawNeedleShadow: p, drawValueBox: x, verifyError: s, prepareTicks: c, drawShadow: b, font: w, normalizedValue: T, formatContext: d }, Oe = Math.PI, Ae = Oe / 2, Pe = Object.assign({}, we, { ticksAngle: 270, startAngle: 45, colorNeedleCircleOuter: "#f0f0f0", colorNeedleCircleOuterEnd: "#ccc", colorNeedleCircleInner: "#e8e8e8", colorNeedleCircleInnerEnd: "#f5f5f5", needleCircleSize: 10, needleCircleInner: true, needleCircleOuter: true, needleStart: 20, animationTarget: "needle", useMinPath: false, barWidth: 0, barStartPosition: "left" }), Me = function(e2) {
        function t2(e3) {
          return o(this, t2), e3 = Object.assign({}, Pe, e3 || {}), i(this, (t2.__proto__ || Object.getPrototypeOf(t2)).call(this, t2.configure(e3)));
        }
        return r(t2, e2), ue(t2, [{ key: "draw", value: function() {
          try {
            var e3 = this.canvas, i2 = [-e3.drawX, -e3.drawY, e3.drawWidth, e3.drawHeight], r2 = i2[0], o2 = i2[1], n2 = i2[2], a2 = i2[3], l2 = this.options;
            if ("needle" === l2.animationTarget) {
              if (!e3.elementClone.initialized) {
                var s2 = e3.contextClone;
                s2.clearRect(r2, o2, n2, a2), s2.save(), this.emit("beforePlate"), W(s2, l2), this.emit("beforeHighlights"), O(s2, l2), this.emit("beforeMinorTicks"), A(s2, l2), this.emit("beforeMajorTicks"), M(s2, l2), this.emit("beforeNumbers"), j(s2, l2), this.emit("beforeTitle"), N(s2, l2), this.emit("beforeUnits"), E(s2, l2), e3.elementClone.initialized = true;
              }
              this.canvas.commit(), e3.context.clearRect(r2, o2, n2, a2), e3.context.save(), e3.context.drawImage(e3.elementClone, r2, o2, n2, a2), e3.context.save(), this.emit("beforeProgressBar"), D(e3.context, l2), this.emit("beforeValueBox"), R(e3.context, l2, z(this)), this.emit("beforeNeedle"), _(e3.context, l2);
            } else {
              var d2 = -We.radians((l2.value - l2.minValue) / (l2.maxValue - l2.minValue) * l2.ticksAngle);
              if (e3.context.clearRect(r2, o2, n2, a2), e3.context.save(), this.emit("beforePlate"), W(e3.context, l2), e3.context.rotate(d2), this.emit("beforeHighlights"), O(e3.context, l2), this.emit("beforeMinorTicks"), A(e3.context, l2), this.emit("beforeMajorTicks"), M(e3.context, l2), this.emit("beforeNumbers"), j(e3.context, l2), this.emit("beforeProgressBar"), D(e3.context, l2), e3.context.rotate(-d2), e3.context.save(), !e3.elementClone.initialized) {
                var c2 = e3.contextClone;
                c2.clearRect(r2, o2, n2, a2), c2.save(), this.emit("beforeTitle"), N(c2, l2), this.emit("beforeUnits"), E(c2, l2), this.emit("beforeNeedle"), _(c2, l2), e3.elementClone.initialized = true;
              }
              e3.context.drawImage(e3.elementClone, r2, o2, n2, a2);
            }
            this.emit("beforeValueBox"), R(e3.context, l2, z(this)), ce(t2.prototype.__proto__ || Object.getPrototypeOf(t2.prototype), "draw", this).call(this);
          } catch (e4) {
            We.verifyError(e4);
          }
          return this;
        } }, { key: "value", set: function(e3) {
          e3 = Se.ensureValue(e3, this.options.minValue), this.options.animation && 360 === this.options.ticksAngle && this.options.useMinPath && (this._value = e3, e3 = this.options.value + ((e3 - this.options.value) % 360 + 540) % 360 - 180), he(t2.prototype.__proto__ || Object.getPrototypeOf(t2.prototype), "value", e3, this);
        }, get: function() {
          return ce(t2.prototype.__proto__ || Object.getPrototypeOf(t2.prototype), "value", this);
        } }], [{ key: "configure", value: function(e3) {
          return e3.barWidth > 50 && (e3.barWidth = 50), isNaN(e3.startAngle) && (e3.startAngle = 45), isNaN(e3.ticksAngle) && (e3.ticksAngle = 270), e3.ticksAngle > 360 && (e3.ticksAngle = 360), e3.ticksAngle < 0 && (e3.ticksAngle = 0), e3.startAngle < 0 && (e3.startAngle = 0), e3.startAngle > 360 && (e3.startAngle = 360), e3;
        } }]), t2;
      }(Se);
      void 0 !== e && (e.RadialGauge = Me), Se.initialize("RadialGauge", Pe);
      var Be = Object.assign({}, we, { borderRadius: 0, barBeginCircle: 30, colorBarEnd: "", colorBarProgressEnd: "", needleWidth: 6, tickSide: "both", needleSide: "both", numberSide: "both", ticksWidth: 10, ticksWidthMinor: 5, ticksPadding: 5, barLength: 85, fontTitleSize: 26, highlightsWidth: 10 }), Ce = function(e2) {
        function n2(e3) {
          return o(this, n2), e3 = Object.assign({}, Be, e3 || {}), i(this, (n2.__proto__ || Object.getPrototypeOf(n2)).call(this, n2.configure(e3)));
        }
        return r(n2, e2), ue(n2, [{ key: "draw", value: function() {
          try {
            var e3 = this.canvas, i2 = [-e3.drawX, -e3.drawY, e3.drawWidth, e3.drawHeight], r2 = i2[0], o2 = i2[1], a2 = i2[2], l2 = i2[3], s2 = this.options;
            if (!e3.elementClone.initialized) {
              var d2 = e3.contextClone;
              d2.clearRect(r2, o2, a2, l2), d2.save(), this.emit("beforePlate"), this.drawBox = F(d2, s2, r2, o2, a2, l2), this.emit("beforeBar"), U.apply(void 0, [d2, s2].concat(t(this.drawBox))), e3.context.barDimensions = d2.barDimensions, this.emit("beforeHighlights"), J(d2, s2), this.emit("beforeMinorTicks"), ee(d2, s2), this.emit("beforeMajorTicks"), K(d2, s2), this.emit("beforeNumbers"), te(d2, s2), this.emit("beforeTitle"), ie(d2, s2), this.emit("beforeUnits"), re(d2, s2), e3.elementClone.initialized = true;
            }
            this.canvas.commit(), e3.context.clearRect(r2, o2, a2, l2), e3.context.save(), e3.context.drawImage(e3.elementClone, r2, o2, a2, l2), e3.context.save(), this.emit("beforeProgressBar"), H.apply(void 0, [e3.context, s2].concat(t(this.drawBox))), this.emit("beforeNeedle"), oe(e3.context, s2), this.emit("beforeValueBox"), se.apply(void 0, [e3.context, s2, s2.animatedValue ? this.options.value : this.value].concat(t(this.drawBox))), ce(n2.prototype.__proto__ || Object.getPrototypeOf(n2.prototype), "draw", this).call(this);
          } catch (e4) {
            We.verifyError(e4);
          }
          return this;
        } }], [{ key: "configure", value: function(e3) {
          return e3.barStrokeWidth >= e3.barWidth && (e3.barStrokeWidth = ye(e3.barWidth / 2)), e3.hasLeft = q("right", e3), e3.hasRight = q("left", e3), e3.value > e3.maxValue && (e3.value = e3.maxValue), e3.value < e3.minValue && (e3.value = e3.minValue), Se.configure(e3);
        } }]), n2;
      }(Se);
      void 0 !== e && (e.LinearGauge = Ce), Se.initialize("LinearGauge", Be), "undefined" != typeof module && Object.assign(e, { Collection: l, GenericOptions: we, Animation: ge, BaseGauge: Se, drawings: We, SmartCanvas: pe, DomObserver: be, vendorize: n });
    }("undefined" != typeof module ? module.exports : window);
  }
});

// node_modules/react-canvas-gauges/dist/LinearGauge.js
var require_LinearGauge = __commonJS({
  "node_modules/react-canvas-gauges/dist/LinearGauge.js"(exports) {
    "use strict";
    Object.defineProperty(exports, "__esModule", {
      value: true
    });
    var _createClass = /* @__PURE__ */ function() {
      function defineProperties(target, props) {
        for (var i = 0; i < props.length; i++) {
          var descriptor = props[i];
          descriptor.enumerable = descriptor.enumerable || false;
          descriptor.configurable = true;
          if ("value" in descriptor) descriptor.writable = true;
          Object.defineProperty(target, descriptor.key, descriptor);
        }
      }
      return function(Constructor, protoProps, staticProps) {
        if (protoProps) defineProperties(Constructor.prototype, protoProps);
        if (staticProps) defineProperties(Constructor, staticProps);
        return Constructor;
      };
    }();
    var _react = require_react();
    var _react2 = _interopRequireDefault(_react);
    var _canvasGauges = require_gauge_min();
    function _interopRequireDefault(obj) {
      return obj && obj.__esModule ? obj : { default: obj };
    }
    function _classCallCheck(instance, Constructor) {
      if (!(instance instanceof Constructor)) {
        throw new TypeError("Cannot call a class as a function");
      }
    }
    function _possibleConstructorReturn(self, call) {
      if (!self) {
        throw new ReferenceError("this hasn't been initialised - super() hasn't been called");
      }
      return call && (typeof call === "object" || typeof call === "function") ? call : self;
    }
    function _inherits(subClass, superClass) {
      if (typeof superClass !== "function" && superClass !== null) {
        throw new TypeError("Super expression must either be null or a function, not " + typeof superClass);
      }
      subClass.prototype = Object.create(superClass && superClass.prototype, { constructor: { value: subClass, enumerable: false, writable: true, configurable: true } });
      if (superClass) Object.setPrototypeOf ? Object.setPrototypeOf(subClass, superClass) : subClass.__proto__ = superClass;
    }
    var ReactLinearGauge = function(_React$Component) {
      _inherits(ReactLinearGauge2, _React$Component);
      function ReactLinearGauge2() {
        _classCallCheck(this, ReactLinearGauge2);
        return _possibleConstructorReturn(this, (ReactLinearGauge2.__proto__ || Object.getPrototypeOf(ReactLinearGauge2)).apply(this, arguments));
      }
      _createClass(ReactLinearGauge2, [{
        key: "componentDidMount",
        value: function componentDidMount() {
          var options = Object.assign({}, this.props, {
            renderTo: this.el
          });
          this.gauge = new _canvasGauges.LinearGauge(options).draw();
        }
      }, {
        key: "componentWillReceiveProps",
        value: function componentWillReceiveProps(nextProps) {
          this.gauge.update(nextProps);
        }
      }, {
        key: "render",
        value: function render() {
          var _this2 = this;
          return _react2.default.createElement("canvas", { ref: function ref(canvas) {
            _this2.el = canvas;
          } });
        }
      }]);
      return ReactLinearGauge2;
    }(_react2.default.Component);
    exports.default = ReactLinearGauge;
  }
});

// node_modules/react-canvas-gauges/dist/RadialGauge.js
var require_RadialGauge = __commonJS({
  "node_modules/react-canvas-gauges/dist/RadialGauge.js"(exports) {
    "use strict";
    Object.defineProperty(exports, "__esModule", {
      value: true
    });
    var _createClass = /* @__PURE__ */ function() {
      function defineProperties(target, props) {
        for (var i = 0; i < props.length; i++) {
          var descriptor = props[i];
          descriptor.enumerable = descriptor.enumerable || false;
          descriptor.configurable = true;
          if ("value" in descriptor) descriptor.writable = true;
          Object.defineProperty(target, descriptor.key, descriptor);
        }
      }
      return function(Constructor, protoProps, staticProps) {
        if (protoProps) defineProperties(Constructor.prototype, protoProps);
        if (staticProps) defineProperties(Constructor, staticProps);
        return Constructor;
      };
    }();
    var _react = require_react();
    var _react2 = _interopRequireDefault(_react);
    var _canvasGauges = require_gauge_min();
    function _interopRequireDefault(obj) {
      return obj && obj.__esModule ? obj : { default: obj };
    }
    function _classCallCheck(instance, Constructor) {
      if (!(instance instanceof Constructor)) {
        throw new TypeError("Cannot call a class as a function");
      }
    }
    function _possibleConstructorReturn(self, call) {
      if (!self) {
        throw new ReferenceError("this hasn't been initialised - super() hasn't been called");
      }
      return call && (typeof call === "object" || typeof call === "function") ? call : self;
    }
    function _inherits(subClass, superClass) {
      if (typeof superClass !== "function" && superClass !== null) {
        throw new TypeError("Super expression must either be null or a function, not " + typeof superClass);
      }
      subClass.prototype = Object.create(superClass && superClass.prototype, { constructor: { value: subClass, enumerable: false, writable: true, configurable: true } });
      if (superClass) Object.setPrototypeOf ? Object.setPrototypeOf(subClass, superClass) : subClass.__proto__ = superClass;
    }
    var ReactRadialGauge = function(_React$Component) {
      _inherits(ReactRadialGauge2, _React$Component);
      function ReactRadialGauge2() {
        _classCallCheck(this, ReactRadialGauge2);
        return _possibleConstructorReturn(this, (ReactRadialGauge2.__proto__ || Object.getPrototypeOf(ReactRadialGauge2)).apply(this, arguments));
      }
      _createClass(ReactRadialGauge2, [{
        key: "componentDidMount",
        value: function componentDidMount() {
          var options = Object.assign({}, this.props, {
            renderTo: this.el
          });
          this.gauge = new _canvasGauges.RadialGauge(options).draw();
        }
      }, {
        key: "componentWillReceiveProps",
        value: function componentWillReceiveProps(nextProps) {
          this.gauge.update(nextProps);
        }
      }, {
        key: "render",
        value: function render() {
          var _this2 = this;
          return _react2.default.createElement("canvas", { ref: function ref(canvas) {
            _this2.el = canvas;
          } });
        }
      }]);
      return ReactRadialGauge2;
    }(_react2.default.Component);
    exports.default = ReactRadialGauge;
  }
});

// node_modules/react-canvas-gauges/dist/index.js
var require_dist = __commonJS({
  "node_modules/react-canvas-gauges/dist/index.js"(exports) {
    Object.defineProperty(exports, "__esModule", {
      value: true
    });
    exports.RadialGauge = exports.LinearGauge = void 0;
    var _LinearGauge = require_LinearGauge();
    var _LinearGauge2 = _interopRequireDefault(_LinearGauge);
    var _RadialGauge = require_RadialGauge();
    var _RadialGauge2 = _interopRequireDefault(_RadialGauge);
    function _interopRequireDefault(obj) {
      return obj && obj.__esModule ? obj : { default: obj };
    }
    exports.LinearGauge = _LinearGauge2.default;
    exports.RadialGauge = _RadialGauge2.default;
  }
});
export default require_dist();
/*! Bundled license information:

canvas-gauges/gauge.min.js:
  (*!
   * The MIT License (MIT)
   * 
   * Copyright (c) 2016 Mykhailo Stadnyk <mikhus@gmail.com>
   * 
   * Permission is hereby granted, free of charge, to any person obtaining a copy
   * of this software and associated documentation files (the "Software"), to deal
   * in the Software without restriction, including without limitation the rights
   * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   * copies of the Software, and to permit persons to whom the Software is
   * furnished to do so, subject to the following conditions:
   * 
   * The above copyright notice and this permission notice shall be included in
   * all copies or substantial portions of the Software.
   * 
   * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   * SOFTWARE.
   *
   * @version 2.1.7
   *)
*/
//# sourceMappingURL=react-canvas-gauges.js.map
