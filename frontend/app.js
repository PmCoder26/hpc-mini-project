const API_BASE = 'https://ozone-mundane-subsystem.ngrok-free.dev/api';
let performanceChart;
let currentEntity = 'employees';

const CONFIG = {
    employees: {
        title: 'Employees Directory (Admin)',
        endpoint: '/filter/employees',
        fields: [
            { id: 'emp-field', type: 'select', label: 'Filter Field', options: ['salary', 'age', 'dept_id'] },
            { id: 'emp-op', type: 'select', label: 'Operator', options: ['>', '<', '=='] },
            { id: 'emp-val', type: 'number', label: 'Threshold Value', default: 50000 }
        ],
        tableCols: ['emp_id', 'emp_name', 'age', 'dept_id', 'salary']
    },
    departments: {
        title: 'Corporate Departments',
        endpoint: '/filter/departments',
        fields: [
            { id: 'dept-field', type: 'select', label: 'Filter Field', options: ['dept_id'] },
            { id: 'dept-op', type: 'select', label: 'Operator', options: ['=='] },
            { id: 'dept-val', type: 'number', label: 'Department ID', default: 5 }
        ],
        tableCols: ['dept_id', 'dept_name']
    },
    payroll: {
        title: 'Payroll Audit Logs',
        endpoint: '/filter/payroll',
        fields: [
            { id: 'pay-field', type: 'select', label: 'Filter Field', options: ['net_paid'] },
            { id: 'pay-op', type: 'select', label: 'Operator', options: ['>', '<'] },
            { id: 'pay-val', type: 'number', label: 'Amount', default: 5000 }
        ],
        tableCols: ['payroll_id', 'emp_id', 'net_paid', 'tax_deduction']
    },
    attendance: {
        title: 'Global Attendance Records',
        endpoint: '/filter/attendance',
        fields: [
            { id: 'att-field', type: 'select', label: 'Filter Field', options: ['status'] },
            { id: 'att-op', type: 'select', label: 'Operator', options: ['=='] },
            { id: 'att-val', type: 'select', label: 'Status', options: [
                {val: 1, text: 'Present'}, 
                {val: 0, text: 'Absent'}, 
                {val: 2, text: 'Leave'}
            ]}
        ],
        tableCols: ['log_id', 'emp_id', 'date_timestamp', 'status']
    }
};

document.addEventListener('DOMContentLoaded', () => {
    initChart();
    checkApiStatus();
    switchEntity('employees');

    // Sidebar navigation
    document.querySelectorAll('.nav-menu li').forEach(li => {
        li.addEventListener('click', (e) => {
            document.querySelectorAll('.nav-menu li').forEach(el => el.classList.remove('active'));
            e.target.classList.add('active');
            switchEntity(e.target.dataset.target);
        });
    });

    document.getElementById('run-query-btn').addEventListener('click', executeQuery);
});

async function checkApiStatus() {
    const statusEl = document.getElementById('api-status');
    const cudaEl = document.getElementById('cuda-status');
    try {
        const res = await fetch('http://127.0.0.1:8000/');
        const data = await res.json();
        statusEl.textContent = 'Online';
        statusEl.className = 'status-indicator online';
        cudaEl.textContent = data.cuda_available ? 'Active (GPU)' : 'Mocked (CPU)';
        cudaEl.className = data.cuda_available ? 'status-indicator online' : 'status-indicator offline';
    } catch (err) {
        statusEl.textContent = 'Offline';
        statusEl.className = 'status-indicator offline';
        cudaEl.textContent = 'Unknown';
    }
}

function switchEntity(entityKey) {
    currentEntity = entityKey;
    const config = CONFIG[entityKey];
    document.getElementById('active-entity-title').textContent = config.title;
    
    // Build Controls
    const container = document.getElementById('filter-controls-container');
    container.innerHTML = '';
    
    config.fields.forEach(f => {
        const div = document.createElement('div');
        div.className = 'filter-group';
        const label = document.createElement('label');
        label.textContent = f.label;
        div.appendChild(label);
        
        let input;
        if (f.type === 'select') {
            input = document.createElement('select');
            input.id = f.id;
            f.options.forEach(opt => {
                const option = document.createElement('option');
                if(typeof opt === 'object') {
                    option.value = opt.val;
                    option.textContent = opt.text;
                } else {
                    option.value = opt;
                    option.textContent = opt;
                }
                input.appendChild(option);
            });
        } else {
            input = document.createElement('input');
            input.type = f.type;
            input.id = f.id;
            input.value = f.default;
        }
        div.appendChild(input);
        container.appendChild(div);
    });

    // Build Table Headers
    const thead = document.querySelector('#results-table thead');
    thead.innerHTML = '';
    const tr = document.createElement('tr');
    config.tableCols.forEach(col => {
        const th = document.createElement('th');
        th.textContent = col.toUpperCase().replace('_', ' ');
        tr.appendChild(th);
    });
    thead.appendChild(tr);
    document.querySelector('#results-table tbody').innerHTML = '<tr><td colspan="' + config.tableCols.length + '" style="text-align:center; padding: 2rem;">Ready for query...</td></tr>';
}

async function executeQuery() {
    const config = CONFIG[currentEntity];
    
    // Extract values dynamically based on IDs
    const field = document.getElementById(config.fields[0].id).value;
    const operator = document.getElementById(config.fields[1].id).value;
    const value = document.getElementById(config.fields[2].id).value;

    const payload = {
        field: field,
        operator: operator,
        value: Number(value),
        limit: 50
    };

    const btn = document.getElementById('run-query-btn');
    const btnText = btn.querySelector('span');
    const loader = btn.querySelector('.loader-spinner');
    
    btn.disabled = true;
    btnText.style.display = 'none';
    loader.style.display = 'block';

    try {
        const res = await fetch(API_BASE + config.endpoint, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });
        
        if(!res.ok) {
            const err = await res.json();
            throw new Error(err.detail || 'Query Failed');
        }

        const data = await res.json();
        updateDashboard(data.metrics, data.data_preview, config.tableCols);
    } catch (err) {
        alert('Error: ' + err.message);
    } finally {
        btn.disabled = false;
        btnText.style.display = 'block';
        loader.style.display = 'none';
    }
}

function updateDashboard(metrics, results, cols) {
    // Animate numbers
    animateValue('cpu-time-val', metrics.cpu_time_sec * 1000, ' ms');
    animateValue('gpu-time-val', metrics.gpu_time_sec * 1000, ' ms');
    animateValue('speedup-val', metrics.speedup_factor, ' x');
    
    document.getElementById('matches-found').textContent = metrics.matches_found.toLocaleString();
    document.getElementById('total-rows').textContent = metrics.total_rows_processed.toLocaleString();

    // Update Chart
    performanceChart.data.datasets[0].data = [metrics.cpu_time_sec * 1000, metrics.gpu_time_sec * 1000];
    performanceChart.update();

    // Render Table
    const tbody = document.querySelector('#results-table tbody');
    tbody.innerHTML = '';
    
    if (results.length === 0) {
        tbody.innerHTML = `<tr><td colspan="${cols.length}" style="text-align:center;">No matches found</td></tr>`;
        return;
    }

    results.forEach(row => {
        const tr = document.createElement('tr');
        cols.forEach(col => {
            const td = document.createElement('td');
            td.textContent = row[col];
            tr.appendChild(td);
        });
        tbody.appendChild(tr);
    });
}

function animateValue(id, value, suffix) {
    const obj = document.getElementById(id);
    let startTimestamp = null;
    const duration = 1000;
    const target = Number(value);

    const step = (timestamp) => {
        if (!startTimestamp) startTimestamp = timestamp;
        const progress = Math.min((timestamp - startTimestamp) / duration, 1);
        // easeOutQuart
        const ease = 1 - Math.pow(1 - progress, 4);
        const current = (target * ease).toFixed(1);
        obj.innerHTML = current + `<span style="font-size:0.6em; font-weight:normal;">${suffix}</span>`;
        if (progress < 1) {
            window.requestAnimationFrame(step);
        }
    };
    window.requestAnimationFrame(step);
}

function initChart() {
    const ctx = document.getElementById('performanceChart').getContext('2d');
    
    Chart.defaults.color = '#94a3b8';
    Chart.defaults.font.family = "'Outfit', sans-serif";
    
    performanceChart = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: ['CPU (Serial)', 'GPU (PyCUDA)'],
            datasets: [{
                label: 'Execution Time (ms)',
                data: [0, 0],
                backgroundColor: [
                    'rgba(255, 75, 43, 0.8)',
                    'rgba(0, 234, 144, 0.8)'
                ],
                borderColor: [
                    '#ff4b2b',
                    '#00ea90'
                ],
                borderWidth: 1,
                borderRadius: 6
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: { display: false }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    grid: { color: 'rgba(255,255,255,0.05)' }
                },
                x: {
                    grid: { display: false }
                }
            }
        }
    });
}
