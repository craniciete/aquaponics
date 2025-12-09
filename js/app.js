
// ---------------- Timestamp converter ----------------
function convertTimestamp(ts) {
    if(ts < 10000000000) ts *= 1000; // convert seconds to ms if needed
    return new Date(ts).toLocaleString();
}

// ---------------- Real-time sensor cards ----------------
db.ref("aquaponics/current").on("value", snapshot => {
    const data = snapshot.val();
    if(!data) return;

    document.getElementById("water_value").textContent = data.water + "%";
    document.getElementById("water_condition").textContent = data.water_status;

    document.getElementById("ph_value").textContent = data.ph;
    document.getElementById("ph_condition").textContent = data.ph_status;

    document.getElementById("turbidity_value").textContent = data.turbidity;
    document.getElementById("turbidity_status").textContent = data.turb_status;
});

// ---------------- Logs table: update every 10 minutes ----------------
function updateLogsTable() {
    db.ref("aquaponics/logs").once("value").then(snapshot => {
        const logsData = snapshot.val();
        const tableBody = document.getElementById("logs_table_body");
        tableBody.innerHTML = "";

        if(!logsData) return;

        // Sort logs by timestamp ascending
        const sortedKeys = Object.keys(logsData).sort((a,b) => logsData[a].timestamp - logsData[b].timestamp);

        sortedKeys.forEach((key,index) => {
            const log = logsData[key];
            const readableTime = convertTimestamp(log.timestamp);

            tableBody.innerHTML += `
                <tr>
                    <td>${index+1}</td>
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

// Initial logs load
updateLogsTable();

// Update logs table every 10 minutes
setInterval(updateLogsTable, 600000);
