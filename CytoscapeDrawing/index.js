const DrawGraph = () => {
  let options = {
    container: document.getElementById("cy"),
  };

  const elements = [
    { data: { id: "a" } },
    { data: { id: "b" } },
    { data: { id: "s0" } },
    { data: { id: "s1" } },
    { data: { data: "z0", source: "a", target: "b" } },
    { data: { data: "z1", source: "a", target: "a" } },
    { data: { data: "z1", source: "a", target: "a" } },
    { data: { source: "a", target: "s0" } },
    { data: { source: "s0", target: "s1" } },
    { data: { source: "s1", target: "a" } },
  ];

  options = {
    ...options,
    elements: elements,
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

  const cy = cytoscape(options);

  //   const cy = cytoscape({
  //     container: document.getElementById("cy"),
  //     elements: [
  //       { data: { id: "a" } },
  //       { data: { id: "b" } },
  //       { data: { id: "a-b", source: "a", target: "b" } },
  //     ],
  //     style: [
  //       {
  //         selector: "node",
  //         style: {
  //           "background-color": "#666",
  //           label: "data(id)",
  //         },
  //       },
  //       {
  //         selector: "edge",
  //         style: {
  //           width: 3,
  //           "line-color": "#ccc",
  //           "target-arrow-color": "#ccc",
  //           "target-arrow-shape": "triangle",
  //           "curve-style": "bezier",
  //         },
  //       },
  //     ],
  //     layout: {
  //       name: "grid",
  //     },
  //   });
};

window.onload = DrawGraph;
