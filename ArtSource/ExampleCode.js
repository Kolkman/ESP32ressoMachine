var beer = document.getElementsByClassName('beer-moving-parts'),
    beers = [];

// BeerGlass constructor
function BeerGlass(ele) {
  this.ele = ele;
  this.drp = ele.getAttribute("data-rel-percent");
  // Final percent
  this.frp = 0;
  this.fillable = false;
  // If no data attribute, don't fill (can garbage collect later)
  if(this.drp) {
    this.frp = this.drp / 100;
    this.fillable = true;
  }
  // Current percent
  this.cp = 0;
  // this.rp = 0;
  // Time to fill glass (ms)
  this.fillTime = 5000;
  // requestAnimationFrame runs 60 times/s
  this.fillRate = 60 / this.fillTime;
  // Get child elements
  this.tf = ele.getElementsByClassName('beer-top-foam')[0];
  this.tt = ele.getElementsByClassName('beer-top-top')[0];
  this.bb = ele.getElementsByClassName('beer-body' )[0];
  this.originalValues = [
    {
      element: this.ele,
      property: 'transform',
      value: this.ele.getAttribute('transform') || ""
    },{
      element: this.tf,
      property: 'd',
      value: this.tf.getAttribute('d') || ""
    },{
      element: this.tt,
      property: 'd',
      value: this.tt.getAttribute('d') || ""
    },{
      element: this.tt,
      property: 'fill',
      value: this.tt.getAttribute('fill') || ""
    },{
      element: this.bb,
      property: 'width',
      value: this.bb.getAttribute('width') || ""
    },{
      element: this.bb,
      property: 'height',
      value: this.bb.getAttribute('height') || ""
    }
  ]
}

// BeerGlass prototype
BeerGlass.prototype = {
  reset: function(empty) {
    console.log(empty);
    var obj = this,
        ovs = obj.originalValues;
    for(var i=0,j=ovs.length;i<j;i++) {
      var ov = ovs[i];
      ov.element.setAttribute(ov.property, ov.value);
    }
    console.log('resetting');
    obj.drp = obj.ele.getAttribute("data-rel-percent");
    if(obj.drp) {
      obj.frp = obj.drp / 100;
      obj.fillable = true;
    }
    obj.cp = 0;
    if(empty) {
      obj.ele.style.display = "none";
    }
  },
  fillErUp: function() {
    var obj = this;
    obj.cp += obj.frp * obj.fillRate;
    var rp = obj.cp;
    if(obj.frp < obj.cp) { 
      rp = obj.frp;
      obj.fillable = false;
    }
    // this.rp = this.cp < this.frp ? this.cp : this.frp;
    if(rp == 0) {return;} else {
      obj.ele.style.display = "";
      var tf = obj.tf;
      var tt = obj.tt;
      var bb = obj.bb;
    };``
      var gw = 80*rp+182,
          bc = 34-(34*rp),
          mc = 34-(44*rp),
          tc = -(28-18*rp),
          fh = 0,
          bh = 30+(430*rp),
          clr = "#FFC84D";
      if(rp > 0.19) {
        fh = 50*rp;
        clr = "#FFFFFF";
        tc = -(23-18*rp);
      }

      var ts = "translate(" + (38-38*rp) + " " + (396-396*rp) + ")";
      var df = "M68,120q" + 1/2*gw + " " + mc + ", " + gw + " 0v" + fh + "q" + -(1/2*gw) + " " + bc + ", " + -gw + " 0Z";
      var dt = "M68,120q" + 1/2*gw + " " + mc + ", " + gw + " 0q" + -(1/2*gw) + " " + tc + ", " + -gw + " 0Z";

      obj.ele.setAttribute("transform", ts);
      tf.setAttribute("d", df);
      tt.setAttribute("d", dt);
      tt.setAttribute("fill", clr);
      bb.setAttribute("width", (gw+4));
      bb.setAttribute("height", bh);
      
      if(obj.fillable) {
        window.requestAnimationFrame(function(){obj.fillErUp();});
      }
    }
  }
};

function makeGlasses() {
  for(var i=0,j=beer.length;i<j;i++) {
    beers.push(new BeerGlass(beer[i]));
  }
  console.log(beers);
} 
makeGlasses();

function pourBeers() {
  for(var i=0,j=beers.length;i<j;i++) {
    if(beers[i].fillable) {
      beers[i].fillErUp();
    }
  }
}
pourBeers();

var pt = document.getElementById('percent-tester'),
    pd = document.getElementsByClassName('graph-data-percent')[0].getElementsByClassName('percent-number')[0];

pt.addEventListener('change', function(){
  var newRPValue = this.value;
  for(var i=0,j=beers.length;i<j;i++) {
    beers[i].ele.setAttribute('data-rel-percent', newRPValue);
    var empty = newRPValue == 0 ? true : false;
    console.log(newRPValue);
    beers[i].reset(empty);
  }
  pourBeers();
});

pt.addEventListener('input', function(){
  var newRPValue = this.value;
  pd.innerText = newRPValue.toString();
});