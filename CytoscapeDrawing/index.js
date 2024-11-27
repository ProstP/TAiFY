var graph;

const OpenAndDrawGraph = (e) => {
  const text = ReadDataFromFile(e, (text) => {
    CreateGraph(text);
    DrawGraph();
  });
};

const CreateGraph = (text) => {
  const lines = text.split("\r\n");

  const firstLine = lines[0].split(";");
  console.log(firstLine);

  const states = [];

  for (i = 0; i < firstLine.length; i++) {
    if (firstLine[i] !== "") {
      states.push(firstLine[i]);
    }
  }

  const edges = [];
  for (i = 1; i < lines.length; i++) {
    const line = lines[i].split(";");

    const inSymbol = line[0];

    for (j = 1; j < line.length; j++) {
      if (line[j] === "-") {
        continue;
      }

      edges.push({
        data: inSymbol,
        from: states[j - 1],
        to: line[j],
      });
    }
  }

  graph = { states, edges };
};

const ReadDataFromFile = (e, processText) => {
  if (e.target.files === null) {
    return;
  }

  const file = e.target.files[0];
  if (!file) {
    return;
  }

  const reader = new FileReader();
  reader.onload = (e) => {
    processText(e.target.result);
  };

  reader.readAsText(file);
};

const DrawGraph = () => {
  let options = {
    container: document.getElementById("cy"),
  };

  const nodeStyle = {
    selector: "node",
    style: {
      "background-color": "#666",
      label: "data(id)",
    },
  };

  const arrowStyle = {
    selector: "edge",
    style: {
      label: "data(data)",
      width: 3,
      "line-color": "#ccc",
      "target-arrow-color": "#ccc",
      "target-arrow-shape": "triangle",
      "curve-style": "bezier",
    },
  };

  const layoutStyle = {
    name: "random",
  };

  options = {
    ...options,
    style: [nodeStyle, arrowStyle],
    layout: layoutStyle,
  };

  let elements = [];

  for (i = 0; i < graph.states.length; i++) {
    elements.push({
      data: { id: graph.states[i] },
    });
  }
  for (i = 0; i < graph.edges.length; i++) {
    elements.push({
      data: {
        data: graph.edges[i].data,
        source: graph.edges[i].from,
        target: graph.edges[i].to,
      },
    });
  }

  options = {
    ...options,
    elements: elements,
  };

  const cy = cytoscape(options);
};

window.onload = () => {
  const input = document.getElementById("input");

  input.onchange = OpenAndDrawGraph;

  const refershBtn = document.getElementById("refresh");
  refershBtn.onclick = DrawGraph;
};
