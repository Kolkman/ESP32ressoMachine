

/// Global Variables

// Determines the size of the time domain
const displayInterval = 15 // in minutes


//Geometry
var margin = { top: 10, right: 60, bottom: 30, left: 60 };
var width = 1500 - margin.left - margin.right;
var height = 300 - margin.top - margin.bottom;
var new_time = new Date().getTime();
var old_time = new Date().getTime();

var toTime = d3.scaleLinear()
  .range(-displayInterval, 0)
var xScale = d3.scaleLinear()
  .range([0, width - margin.left - margin.right])
var yScale = d3.scaleLinear()
  .range([height - margin.top - margin.bottom, 0])
var y2Scale = d3.scaleLinear()
  .range([height - margin.top - margin.bottom, 0])
var dataStore = [];

var para;
var svg;

fetch(url + '/api/v1/get?PidInterval')
  .then(function (response) {
    return response.json()
  })
  .then(function (data) {
    var p = data.PidInterval
  })
  .catch(function (err) {
    console.log('error' + err);
  });

// From Brendan Sudol
// https://brendansudol.com/writing/responsive-d3
function responsivefy(svg) {
  // get container + svg aspect ratio
  var container = d3.select(svg.node().parentNode),
    width = parseInt(svg.style("width")),
    height = parseInt(svg.style("height")),
    aspect = width / height;

  // add viewBox and preserveAspectRatio properties,
  // and call resize so that svg resizes on inital page load
  svg.attr("viewBox", "0 0 " + width + " " + height)
    .attr("perserveAspectRatio", "xMinYMid")
    .call(resize);

  // to register multiple listeners for same event type, 
  // you need to add namespace, i.e., 'click.foo'
  // necessary if you call invoke this function for multiple svgs
  // api docs: https://github.com/mbostock/d3/wiki/Selections#on
  d3.select(window).on("resize." + container.attr("id"), resize);

  // get width of container and resize svg to fit it
  function resize() {
    var targetWidth = parseInt(container.style("width"));
    svg.attr("width", targetWidth);
    svg.attr("height", Math.round(targetWidth / aspect));
  }
}


window.addEventListener("load", function () {
  console.log("Window Loaded");

  d3.json(url + '/api/v1/statistics').then(function (data) {
    // Handle dimensions
    svg = d3.select("#graph")
      .append("svg")
      .attr("width", width)// + margin.left + margin.right)
      .attr("height", height + margin.top + margin.bottom)
      .call(responsivefy)
      .append("g")
      .attr("transform",
        "translate(" + margin.left + "," + margin.top + ")");



    // text label for the x axis
    svg.append("text")
      .attr("transform",
        "translate(" + (width / 2) + " ," +
        (height + margin.top) + ")")
      .style("text-anchor", "middle")
      .text("relative time (min)")
      .attr("font-size", "110%");

    // text label for the left y axis

    svg.append("text")
      .attr("transform",
        "translate(" + (0 - margin.left) + " ," +
        (height / 2) + "),rotate(-90)")
      .style("text-anchor", "middle")
      .attr("dy", "1em")
      .style("text-anchor", "middle")
      .text("Temperature (\u00B0C)")
      .attr("font-size", "110%");
    // text for the right y axis
    svg.append("text")
      .attr("transform",
        "translate(" + (width - margin.right) + " ," +
        (height / 2 + margin.top) + "),rotate(90)")
      .style("text-anchor", "middle")
      .attr("dy", "1em")
      .text("Power")
      .attr("font-size", "110%");

    dataStore = data;
    CalculateAxes(dataStore)
    drawAxes();
    init_visualize(dataStore);
    old_time = new Date().getTime();
  });
});


// Listen for messages
var source = new EventSource(url + '/events');
//var source = new EventSource('/events');


source.addEventListener('open', function (e) {
  console.log("Events Connected");
}, false);
source.addEventListener('error', function (e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
}, false);
source.addEventListener('status', function (e) {
  //console.log("Status:", e.data);
  const parseddata = (JSON.parse(e.data));
  var object = {};
  object.t = parseddata.time;
  object.T = parseddata.measuredTemperature;
  object.p = parseddata.heaterPower;
  dataStore.push(object);



  new_time = new Date().getTime();
  CalculateAxes(dataStore);
  deleteAxes();
  drawAxes();
  visualize(dataStore);
  while ((object.t - dataStore[0].t) > displayInterval * 60 * 1000) {  //This breaks at time roll.
    dataStore.shift();
  }
  old_time = new_time;
}, false);


function CalculateAxes(data_in) {
  // mostly following https://d3-graph-gallery.com/graph/line_basic.html
  data_in = data_in.sort(function (a, b) { return parseInt(a.t) - parseInt(b.t) })

  xMax = d3.max(data_in, function (d) { return parseInt(d.t); });

  xScale.domain([-displayInterval, 0]);

  yScaleMin = d3.min(data_in, function (d) {
    return parseFloat(d.T);
  });
  yScaleMax = d3.max(data_in, function (d) {
    return parseFloat(d.T);
  });
  y2ScaleMin = d3.min(data_in, function (d) {
    return parseFloat(d.p);
  });
  y2ScaleMax = d3.max(data_in, function (d) {
    return parseFloat(d.p);
  });
  yScaleMin -= .1;
  yScaleMax += .1;
  y2ScaleMin -= 50;
  y2ScaleMax += 40;
  if (y2ScaleMin < 0) y2ScaleMin = 0;
  yScale.domain([yScaleMin, yScaleMax]);
  y2Scale.domain([y2ScaleMin, y2ScaleMax]);
}


function drawAxes() {
  svg.append("g").attr("class", "x axis")
    .attr("transform", "translate(0," + (height - margin.top - margin.bottom) + ")")
    .call(d3.axisBottom(xScale));
  svg.append("g").attr("class", "y axis")
    .call(d3.axisLeft(yScale));
  svg.append("g").attr("class", "y2 axis")
    .attr("transform", "translate(" + (width - margin.right - margin.left) + ",0)")
    .call(d3.axisRight(y2Scale));

}

function deleteAxes() {
  svg.selectAll(".y.axis").remove();
  svg.selectAll(".y2.axis").remove();
  svg.selectAll(".x.axis").remove();
}



function init_visualize(data_in) {
  svg.append("defs").append("clipPath")
    .attr("id", "clip")
    .append("rect")
    .attr("width", width)
    .attr("height", height);
  svg.append("g").attr("clip-path", "url(#clip)")
    .append("path")
    .attr("id", "temperaturePath")
    .datum(data_in)
    .join()
    .attr("fill", "none")
    .attr("stroke", "steelblue")
    .attr("stroke-width", 2.5)
    .attr("d", d3.line()
      .x(function (d) {
        return xScale((parseInt(d.t) - xMax) / (60 * 1000))
      })
      .y(function (d) { return yScale(parseFloat(d.T)); })
    )

  //plot the power graph
  svg.append("path")
    .attr("id", "powerPath")
    .datum(data_in)
    .attr("fill", "none")
    .attr("stroke", "red")
    .attr("opacity", 0.5)
    .attr("stroke-width", 2.5)
    .attr("d", d3.line()
      .x(function (d) {
        return xScale((parseInt(d.t) - xMax) / (60 * 1000))
      })
      .y(function (d) { return y2Scale(parseFloat(d.p)); })
    )
}

function visualize(data_in) {


  d3.select("#temperaturePath")
    .datum(data_in)
    .join()

    .attr("d", d3.line()
      .x(function (d) {
        return xScale((parseInt(d.t) - xMax) / (60 * 1000))
      })
      .y(function (d) { return yScale(parseFloat(d.T)); })
    )
    .attr("transform", null)
    .transition().duration(Math.abs(new_time - old_time) * .90).ease(d3.easeLinear)
    .attr("transform", "translate(" + xScale(-displayInterval - (new_time - old_time) / (1000 * 60)) + ")");



  //plot the power graph
  d3.select("#powerPath")
    .datum(data_in)

    .attr("d", d3.line()
      .x(function (d) {
        return xScale((parseInt(d.t) - xMax) / (60 * 1000))
      })
      .y(function (d) { return y2Scale(parseFloat(d.p)); })
    )
    .attr("transform", null)
    .transition().duration(Math.abs(new_time - old_time - 20) * .90).ease(d3.easeLinear)
    .attr("transform", "translate(" + xScale(-displayInterval - (new_time - old_time) / (1000 * 60)) + ")");


}



