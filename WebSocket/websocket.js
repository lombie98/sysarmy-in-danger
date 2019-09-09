$(function() {
    var loadChart = createChart($("#loadChart"));
    window.loadChart = loadChart;

    var url = "ws://localhost:5555"
    var ws = new WebSocket(url);

    ws.onmessage = changeDivData;
});

function changeDivData(evt) {
    console.log("Received WebSocker Event:");
    console.log(evt);
    var node = $(".dataContainer");
    node.empty();
    var newData = $("<p>");
    newData.html(evt.data);
    newData.appendTo(node);

    var data = evt.data.split(";");
    updateChart(window.loadChart, data);
}

function createChart(chartContainer) {
    var labels = [];
    var oneMinDataset = {
        label: "1 Min",
        backgroundColor: "rgba(255, 0, 0, 0.2)",
        borderColor: "red",
        data: [],
    }
    var fiveMinDataset = {
        label: "5 Min",
        backgroundColor: "rgba(255, 255, 0, 0.2)",
        borderColor: "yellow",
        data: [],
    }
    var fifteenMinDataset = {
        label: "15 Min",
        backgroundColor: "rgba(0, 255, 0, 0.2)",
        borderColor: "green",
        data: [],
    }
    var data = {
        labels: labels,
        datasets: [
            oneMinDataset,
            fiveMinDataset,
            fifteenMinDataset
        ]
    }
    var loadChart = new Chart(chartContainer, {
        type: 'line',
        data: data,
        options: {},
    });
    return loadChart
}

function updateChart(chart, data) {
    chart.data.labels.push(moment().format("h:mm:ss"));
    for (var i = 0; i < chart.data.datasets.length; i++) {
        chart.data.datasets[i].data.push(data[i]);
    }
    chart.update();
}
