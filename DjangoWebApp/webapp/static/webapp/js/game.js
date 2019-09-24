$(function() {
    window.gameWebSocket = new WebSocket('ws://' + window.location.host + '/webapp/game/');
    window.gameWebSocket.onmessage = wsMessageReceived;

    setDefaults();
    $(".btnPreStart").click(initBackwardTimer);
});

var conn_qty = 8;
var slot_qty = 32;
var sender_blacklist = [];
var receiver_blacklist = [];
var timeout = 10;

function setDefaults() {
    $("#id_conn_qty").val(conn_qty.toString());
    $("#id_slot_qty").val(slot_qty.toString());
    $("#id_sender_blacklist").val(sender_blacklist.toString());
    $("#id_receiver_blacklist").val(receiver_blacklist.toString());
    $("#id_timeout").val(timeout.toString());
}

function initBackwardTimer() {
    $(".preStart").hide();
    $(".gameBlock").removeClass("d-none");

    var timerCont = $(".backwardTimer");
    var Timer = timerCont.backward_timer({
        seconds: 3,
        format: 's%',
        on_exhausted: timerExhausted,
        no_leading_zero: true,
    });
    timerCont.backward_timer('start');
}

function timerExhausted(backwardTimer) {
    console.log("Timer Out");
    $(".backwardTimer").hide();
    configs = {
        conn_qty: parseInt($("#id_conn_qty").val()),
        slot_qty: parseInt($("#id_slot_qty").val()),
        sender_blacklist: $("#id_sender_blacklist").val().split(','),
        receiver_blacklist: $("#id_receiver_blacklist").val().split(','),
        timeout: parseInt($("#id_timeout").val()),
    }
    var data = {
        signal: "start",
        value: configs,
    }
    window.gameWebSocket.send(JSON.stringify(data));
    initClock();
}

function wsMessageReceived(ev) {
    var data = JSON.parse(ev.data);
    console.log("Received: " + ev.data);
    switch (data.signal) {
        case "status":
            refreshStatus(data.value);
            break;
        case "health_check":
            healthCheck(data.value);
            break;
        case "win":
            renderWin();
            break;
        case "timeout":
            renderTimeOut();
            break;
        case "config":
            renderConfig(data.value);
            break;
        default:
            console.log("Unknown directive received");
    }
}

function renderConfig(state) {
    var confCont = $(".gameConfigContainer");
    var confList = $("<ul>");
    confList.addClass('configList');
    confList.appendTo(confCont);
    for (let i = 0; i < state.conn_qty; i++) {
        let recVal = state.receiver_pos[i];
        let senVal = state.sender_pos[i];
        let newLi = $("<li>");
        newLi.html(senVal + " -> " + recVal);
        newLi.appendTo(confList);
    }
}

function healthCheck(state) {
    var SenderStatus = $(".SenderStatus");
    var ReceiverStatus = $(".ReceiverStatus");
    _setState(SenderStatus, state.sender);
    _setState(ReceiverStatus, state.receiver);
}

function _setState (cont, st) {
    var color = "red";
    if (st === "Y") {
        color = "green";
    } else if (st === "N") {
        color = "red";
    }
    cont.css("color", color);
}

function refreshStatus(state) {
    var statusCont = $(".gameStatusContainer");
    for (let i = 0; i < state.length; i++) {
        let color = state[i] ? 'green' : 'black';
        $(".configList li").eq(i).css('color', color);
    }
}

function renderTimeOut() {
    endGame();
    var gameCont = $(".gameBlock");
    gameCont.empty();
    $(".timeOut").removeClass("d-none");
}

function renderWin() {
    endGame();
    $(".Win").removeClass("d-none");
    $(".gameBlock").addClass("awesomeText");
}

function endGame() {
    stopClock();
}


function msToMS( ms ) {
    var seconds = ms / 1000;
    var minutes = parseInt( seconds / 60 );
    seconds = parseInt(seconds % 60);
    milis = parseInt(ms % 1000);
    return {
        milis: milis,
        secs: seconds,
        minutes: minutes,
    }
}


// START CLOCK SCRIPT

Number.prototype.pad = function(n) {
  for (var r = this.toString(); r.length < n; r = 0 + r);
  return r;
};

function updateClock() {
    var now = new Date();
    var fragments = msToMS(now - window.ClockStart);
    var tags = ["m", "s", "mi"],
        corr = [
            fragments.minutes.pad(2),
            fragments.secs.pad(2),
            fragments.milis.pad(3),
        ];
    for (var i = 0; i < tags.length; i++)
        document.getElementById(tags[i]).firstChild.nodeValue = corr[i];
}

function stopClock() {
    window.clearInterval(window.ClockIntervalID);
}

function initClock() {
    $(".Timer").removeClass('d-none');
    window.ClockStart = new Date();
    updateClock();
    window.ClockIntervalID = window.setInterval("updateClock()", 1);
}

// END CLOCK SCRIPT
