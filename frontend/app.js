const API_BASE = 'https://ozone-mundane-subsystem.ngrok-free.dev';

let performanceChart;
let currentEntity = "employees";

const CONFIG = {
    employees: {
        endpoint: "/api/filter/employees",
        fields: ["emp-val"],
        cols: ["emp_id", "age", "dept_id", "salary"],
        queryField: "salary"
    },
    departments: {
        endpoint: "/api/filter/departments",
        fields: ["dept-val"],
        cols: ["dept_id"],
        queryField: "dept_id"
    },
    payroll: {
        endpoint: "/api/filter/payroll",
        fields: ["pay-val"],
        cols: ["payroll_id", "emp_id", "net_paid", "tax_deduction"],
        queryField: "net_paid"
    },
    attendance: {
        endpoint: "/api/filter/attendance",
        fields: ["att-val"],
        cols: ["emp_id", "status"],
        queryField: "status"
    }
};

document.addEventListener("DOMContentLoaded", () => {
    initChart();

    document.querySelectorAll(".nav-menu li").forEach(li => {
        li.addEventListener("click", () => {
            document.querySelectorAll(".nav-menu li").forEach(el => el.classList.remove("active"));
            li.classList.add("active");
            switchEntity(li.dataset.target);
        });
    });

    document.getElementById("run-query-btn").addEventListener("click", executeQuery);

    switchEntity("employees");
});


// ---------------- SWITCH ----------------
function switchEntity(entity) {
    currentEntity = entity;
    
    // Update titles
    document.getElementById("active-entity-title").innerText = entity.charAt(0).toUpperCase() + entity.slice(1);
    
    // Hide all filters explicitly
    const filters = ["employees-filter", "departments-filter", "payroll-filter", "attendance-filter"];
    filters.forEach(id => {
        document.getElementById(id).style.display = "none";
    });
    
    // Show active filter explicitly
    document.getElementById(entity + "-filter").style.display = "flex";

    // Update table headers dynamically
    const cfg = CONFIG[entity];
    const thead = document.querySelector("#results-table thead");
    thead.innerHTML = "<tr>" + cfg.cols.map(c => `<th>${c}</th>`).join("") + "</tr>";
    
    // Clear previous results
    const tbody = document.querySelector("#results-table tbody");
    tbody.innerHTML = `<tr><td colspan="${cfg.cols.length}">Execute query to view data</td></tr>`;
}


// ---------------- FETCH ----------------
async function executeQuery() {
    const cfg = CONFIG[currentEntity];

    const inputId = cfg.fields[0];
    const value = document.getElementById(inputId).value;

    const payload = {
        field: cfg.queryField,
        value: Number(value),
        limit: 500000
    };

    const btn = document.getElementById("run-query-btn");
    const loader = btn.querySelector(".loader-spinner");
    const span = btn.querySelector("span");

    // Start loading
    btn.disabled = true;
    loader.style.display = "block";
    span.innerText = "Processing...";

    try {
        const res = await fetch(API_BASE + cfg.endpoint, {
            method: "POST",
            headers: { 
                "Content-Type": "application/json",
                "ngrok-skip-browser-warning": "true" 
            },
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
    } finally {
        // Stop loading
        btn.disabled = false;
        loader.style.display = "none";
        span.innerText = "Execute Parallel Query";
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
            responsive: true,
            scales: {
                x: {
                    ticks: {
                        color: "#fff",
                        font: { size: 16, weight: "bold" }
                    }
                },
                y: {
                    ticks: {
                        color: "#fff",
                        font: { size: 14 }
                    }
                }
            },
            plugins: {
                legend: {
                    labels: {
                        color: "#fff",
                        font: { size: 14 }
                    }
                }
            }
        }
    });
}