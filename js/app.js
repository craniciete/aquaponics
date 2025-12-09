function convertTimestamp(ts) {
    if (ts < 10000000000) ts *= 1000;
    return new Date(ts).toLocaleString();
}

db.ref("aquaponics/current").on("value", snapshot => {
    const data = snapshot.val();
    if (!data) return;

    document.getElementById("water_value").textContent = data.water + "%";
    document.getElementById("water_condition").textContent = data.water_status;

    document.getElementById("ph_value").textContent = data.ph;
    document.getElementById("ph_condition").textContent = data.ph_status;

    document.getElementById("turbidity_value").textContent = data.turbidity;
    document.getElementById("turbidity_status").textContent = data.turb_status;

    updateAlerts(data);
});

function updateAlerts(data) {
    const alertList = document.getElementById("alert-list");
    alertList.innerHTML = "";

    if (data.water <= 40) {
        alertList.innerHTML += `
        <div class="alert-box alert-danger">
            <div class="alert-icon"><img src="assets/alert.png" alt="alert-logo"></div>
            <span>Water level is LOW (${data.water}%)</span>
        </div>`;
    } else if (data.water <= 70) {
        alertList.innerHTML += `
        <div class="alert-box alert-warning">
            <div class="alert-icon"><img src="assets/caution.png" alt="caution-logo"></div>
            <span>Water level is MEDIUM (${data.water}%)</span>
        </div>`;
    } else {
        alertList.innerHTML += `
        <div class="alert-box alert-success">
            <div class="alert-icon"><img src="assets/normal.png" alt="normal-logo"></div>
            <span>Water level is HIGH (${data.water}%)</span>
        </div>`;
    }

    if (data.ph_status.toLowerCase().includes("acidic")) {
        alertList.innerHTML += `
        <div class="alert-box alert-danger">
            <div class="alert-icon"><img src="assets/alert.png" alt="alert-logo"></div>
            <span>pH is ACIDIC (${data.ph})</span>
        </div>`;
    } else if (data.ph_status.toLowerCase().includes("neutral")) {
        alertList.innerHTML += `
        <div class="alert-box alert-success">
            <div class="alert-icon"><img src="assets/normal.png" alt="normal-logo"></div>
            <span>pH is NEUTRAL (${data.ph})</span>
        </div>`;
    } else if (data.ph_status.toLowerCase().includes("alkaline")) {
        alertList.innerHTML += `
        <div class="alert-box alert-warning">
            <div class="alert-icon"><img src="assets/caution.png" alt="caution-logo"></div>
            <span>pH is ALKALINE (${data.ph})</span>
        </div>`;
    }

    if (data.turb_status.toLowerCase().includes("dirty")) {
        alertList.innerHTML += `
        <div class="alert-box alert-warning">
            <div class="alert-icon"><img src="assets/caution.png" alt="caution-logo"></div>
            <span>Water is DIRTY (${data.turbidity})</span>
        </div>`;
    } else if (data.turb_status.toLowerCase().includes("cloudy")) {
        alertList.innerHTML += `
        <div class="alert-box alert-danger">
            <div class="alert-icon"><img src="assets/alert.png" alt="alert-logo"></div>
            <span>Water is CLOUDY (${data.turbidity})</span>
        </div>`;
    } else {
        alertList.innerHTML += `
        <div class="alert-box alert-success">
            <div class="alert-icon"><img src="assets/normal.png" alt="normal-logo"></div>
            <span>Water is CLEAR (${data.turbidity})</span>
        </div>`;
    }

    if (alertList.innerHTML.trim() === "") {
        alertList.innerHTML = `<div style="opacity:0.6;font-size:14px;">No alerts</div>`;
    }
}

function updateLogsTable() {
    db.ref("aquaponics/logs").once("value").then(snapshot => {
        const logsData = snapshot.val();
        const tableBody = document.getElementById("logs_table_body");
        tableBody.innerHTML = "";
        if (!logsData) return;

        const sortedKeys = Object.keys(logsData).sort(
            (a, b) => logsData[a].timestamp - logsData[b].timestamp
        );

        sortedKeys.forEach((key, index) => {
            const log = logsData[key];
            const readableTime = convertTimestamp(log.timestamp);

            tableBody.innerHTML += `
                <tr>
                    <td>${index + 1}</td>
                    <td>${readableTime}</td>
                    <td>${log.ph}</td>
                    <td>${log.water}%</td>
                    <td>${log.turbidity}</td>
                    <td>Water: ${log.water_status}, pH: ${log.ph_status}, Turbidity: ${log.turb_status}</td>
                </tr>
            `;
        });
    });
}

updateLogsTable();
setInterval(updateLogsTable, 600000); 

function setMode(mode) {
    if (!mode) return;
    db.ref("aquaponics/mode").set(mode)
        .then(() => {
            console.log("Mode set to:", mode);
            document.getElementById("system_mode").textContent = mode;
        })
        .catch((err) => console.error("Error setting mode:", err));
}

db.ref("aquaponics/mode").on("value", snapshot => {
    const mode = snapshot.val();
    if (mode) document.getElementById("system_mode").textContent = mode;
});