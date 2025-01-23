var graph;

const OpenAndDrawGraph = (e) => {
  const text = ReadDataFromFile(e, (text) => {
    CreateGraph(text);
    DrawGraph();
  });
};

const CreateGraph = (text) => {
  const lines = text.split("\r\n");

  const isFirstOut = lines[1][0] === ";";
  const firstLine = lines[0].split(";");
  const secondLine = isFirstOut ? lines[1].split(";") : null;
  console.log(lines);

  const states = [];

  const lineWithStates = isFirstOut ? [...secondLine] : [...firstLine];
  for (let i = 0; i < lineWithStates.length; i++) {
    if (lineWithStates[i] !== "") {
      states.push(lineWithStates[i]);
    }
  }

  console.log(states);
  console.log(lines);

  const outSymbols = [];
  if (isFirstOut) {
    for (let i = 0; i < firstLine.length; i++) {
      if (firstLine[i] !== "") {
        outSymbols.push(firstLine[i]);
      }
    }
  }

  console.log(states);
  console.log(lines);

  const edges = [];
  for (i = isFirstOut ? 2 : 1; i < lines.length; i++) {
    const line = lines[i].split(";");

    const inSymbol = line[0];

    for (j = 1; j < line.length; j++) {
      if (line[j] === "-") {
        continue;
      }

      const from = isFirstOut
        ? states[j - 1] + "/" + outSymbols[j - 1]
        : states[j - 1];

      const toState = isFirstOut
        ? line[j] + "/" + outSymbols[states.indexOf(line[j])]
        : line[j];

      edges.push({
        data: inSymbol,
        from: from,
        to: toState,
      });
    }
  }

  if (isFirstOut) {
    for (let i = 0; i < states.length; i++) {
      for (let j = 2; j < lines.length; j++) {
        lines[j].replace(states[i], states[i] + "/" + outSymbols[i]);
      }

      states[i] = states[i] + "/" + outSymbols[i];
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
