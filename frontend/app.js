const API_BASE = 'https://ozone-mundane-subsystem.ngrok-free.dev/api';

let performanceChart;
let currentEntity = "employees";

const CONFIG = {
    employees: {
        endpoint: "/api/filter/employees",
        fields: ["emp-val"],
        cols: ["emp_id", "age", "dept_id", "salary"]
    },
    departments: {
        endpoint: "/api/filter/departments",
        fields: ["dept-val"],
        cols: ["dept_id"]
    },
    payroll: {
        endpoint: "/api/filter/payroll",
        fields: ["pay-val"],
        cols: ["payroll_id", "emp_id", "net_paid", "tax_deduction"]
    },
    attendance: {
        endpoint: "/api/filter/attendance",
        fields: ["att-val"],
        cols: ["emp_id", "status"]
    }
};

document.addEventListener("DOMContentLoaded", () => {
    initChart();

    document.querySelectorAll(".nav-menu li").forEach(li => {
        li.addEventListener("click", () => {
            switchEntity(li.dataset.target);
        });
    });

    document.getElementById("run-query-btn").addEventListener("click", executeQuery);

    switchEntity("employees");
});


// ---------------- SWITCH ----------------
function switchEntity(entity) {
    currentEntity = entity;
    document.getElementById("active-entity-title").innerText = entity;
}


// ---------------- FETCH ----------------
async function executeQuery() {
    const cfg = CONFIG[currentEntity];

    const inputId = cfg.fields[0];
    const value = document.getElementById(inputId).value;

    const payload = {
        field: "auto",
        value: Number(value),
        limit: 50
    };

    try {
        const res = await fetch(API_BASE + cfg.endpoint, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(payload)
        });

        if (!res.ok) {
            const err = await res.json();
            throw new Error(err.detail || "Request failed");
        }

        const data = await res.json();

        updateUI(data, cfg.cols);

    } catch (err) {
        console.error("FETCH ERROR:", err);
        alert("Error: " + err.message);
    }
}


// ---------------- UI UPDATE ----------------
function updateUI(data, cols) {

    const metrics = data.metrics;
    const rows = data.data_preview;

    document.getElementById("cpu-time-val").innerText =
        (metrics.cpu_time_sec * 1000).toFixed(2) + " ms";

    document.getElementById("gpu-time-val").innerText =
        (metrics.gpu_time_sec * 1000).toFixed(2) + " ms";

    document.getElementById("speedup-val").innerText =
        metrics.speedup_factor + "x";

    document.getElementById("matches-found").innerText =
        metrics.matches_found;

    document.getElementById("total-rows").innerText =
        metrics.total_rows_processed;


    // Chart safe update
    if (performanceChart) {
        performanceChart.data.datasets[0].data = [
            metrics.cpu_time_sec * 1000,
            metrics.gpu_time_sec * 1000
        ];
        performanceChart.update();
    }

    // Table render
    const tbody = document.querySelector("#results-table tbody");
    tbody.innerHTML = "";

    if (!rows || rows.length === 0) {
        tbody.innerHTML = "<tr><td>No results</td></tr>";
        return;
    }

    rows.forEach(r => {
        const tr = document.createElement("tr");

        cols.forEach(c => {
            const td = document.createElement("td");
            td.innerText = r[c] ?? "-";
            tr.appendChild(td);
        });

        tbody.appendChild(tr);
    });
}


// ---------------- CHART ----------------
function initChart() {
    const ctx = document.getElementById("performanceChart");

    performanceChart = new Chart(ctx, {
        type: "bar",
        data: {
            labels: ["CPU", "GPU"],
            datasets: [{
                label: "Time (ms)",
                data: [0, 0]
            }]
        },
        options: {
            responsive: true
        }
    });
}