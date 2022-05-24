
const displayInterval = 10 // in minutes

var margin = { top: 10, right: 30, bottom: 30, left: 60 };
var width = 1500 - margin.left - margin.right;
var height = 300 - margin.top - margin.bottom;


var toTime = d3.scaleLinear()
  .range(-15, 0)
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
  d3.json(url + '/api/v1/statistics', function (error, data) {
    if (error) throw error;
    svg = d3.select("#graph")
      .append("svg")
      .attr("width", width + margin.left + margin.right)
      .attr("height", height + margin.top + margin.bottom)
      .call(responsivefy)
      .append("g")
      .attr("transform",
        "translate(" + margin.left + "," + margin.top + ")");



    // text label for the x axis
    svg.append("text")
      .attr("transform",
        "translate(" + (width / 2) + " ," +
        (height + margin.top + 20) + ")")
      .style("text-anchor", "middle")
      .text("relative time (min)")
      .attr("font-size", "130%");

    // text label for the y axis

    svg.append("text")
      .attr("transform",
        "translate(" + (0 - margin.left) + " ," +
        (height / 2) + "),rotate(-90)")
      .style("text-anchor", "middle")
      .attr("dy", "1em")
      .style("text-anchor", "middle")
      .text("Temperature \u2103")
      .attr("font-size", "130%");

    svg.append("text")
      .attr("transform",
        "translate(" + (width - margin.right) + " ," +
        (height / 2 + margin.top) + "),rotate(90)")
      .style("text-anchor", "middle")
      .attr("dy", "1em")
      .text("Power")
      .attr("font-size", "130%");

    dataStore = data;
    //console.log(dataStore);
    visualize(dataStore);
  });
}
);

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


  while ((object.t-dataStore[0].t)>displayInterval*60*1000) {  //This breaks at time roll.
    dataStore.shift();
  }
  visualize(dataStore);
}, false);


function visualize(data_in) {

  // mostly following https://d3-graph-gallery.com/graph/line_basic.html
  data_in = data_in.sort(function (a, b) { return parseInt(a.t) - parseInt(b.t) })
  xMax = d3.max(data_in, function (d) { return parseInt(d.t); });



  //   return parseInt(d.t);
  //xScale.domain(d3.extent(data_in, function (d) {
  //   return parseInt(d.t);
  //}));

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




  //create x axis
  // remove any previously drawn axis
  svg.selectAll(".x.axis").remove();
  // draw the new axis
  svg.append("g").attr("class", "x axis")
    .attr("transform", "translate(0," + (height - margin.top - margin.bottom) + ")")
    .call(d3.axisBottom(xScale));
  // create temperature  axis
  // remove any previously drawn axis
  svg.selectAll(".y.axis").remove();
  // draw the new axis
  svg.append("g").attr("class", "y axis")
    .call(d3.axisLeft(yScale));

  // create power Axis
  // remove any previously drawn axis
  svg.selectAll(".y2.axis").remove();
  // draw the new axis
  svg.append("g").attr("class", "y2 axis")
    .attr("transform", "translate(" + (width - margin.right - margin.left) + ",0)")
    .call(d3.axisRight(y2Scale));


  //var lines_t = svg.selectAll(".line_t").data(data_in).attr("class", "line_t")

  // remove any previously drawn line
  svg.selectAll(".line_t").remove();
  svg.selectAll(".line_p").remove();



  // Plot the temperature graph
  svg.append("path")
    .datum(data_in)
    .attr("class", "line_t")
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
    .datum(data_in)
    .transition().duration(200)
    .attr("class", "line_p")
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


