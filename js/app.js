db.ref("aquaponics/logs/0").on("value", snapshot => {
    const data = snapshot.val();
    if (!data) return;

    document.getElementById("water_value").textContent = data.water + " %";
    document.getElementById("ph_value").textContent = data.ph;
    document.getElementById("ph_condition").textContent =
        data.ph < 6.8 ? "Acidic" :
        data.ph > 7.2 ? "Basic" : "Neutral";

    document.getElementById("turbidity_value").textContent = data.turbidity + " NTU";
    document.getElementById("turbidity_status").textContent = data.turb_status;
});
