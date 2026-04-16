import csv
import os
import matplotlib.pyplot as plt
import numpy as np

def read_metrics_from_csv():
    filepath = 'data/measurements.csv'
    if not os.path.exists(filepath):
        print(f"ERROR: {filepath} not found! Please run your ./hpc_engine and execute some queries first.")
        return [], [], [], []
    
    # We will average the metrics in case the user successfully ran the exact same query multiple times!
    data_dict = {}
    
    with open(filepath, 'r') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            q_name = row['Query_Name']
            if q_name not in data_dict:
                data_dict[q_name] = {'s_time': [], 'p_time': [], 'speedup': []}
                
            data_dict[q_name]['s_time'].append(float(row['Serial_Time_ms']))
            data_dict[q_name]['p_time'].append(float(row['Parallel_Time_ms']))
            data_dict[q_name]['speedup'].append(float(row['Speedup']))
            
    # Calculate the Averages
    queries = []
    serial_times = []
    parallel_times = []
    speedups = []
    
    for q_name, metrics in data_dict.items():
        queries.append(q_name)
        serial_times.append(sum(metrics['s_time']) / len(metrics['s_time']))
        parallel_times.append(sum(metrics['p_time']) / len(metrics['p_time']))
        speedups.append(sum(metrics['speedup']) / len(metrics['speedup']))
        
    return queries, serial_times, parallel_times, speedups

def generate_graphs(queries, serial_times, parallel_times, speedups):
    os.makedirs('results', exist_ok=True)
    
    # Clean up long query names for the Y-axis
    short_queries = [q.split('(')[0].strip() for q in queries]
    
    y = np.arange(len(short_queries))
    height = 0.35

    # -----------------------------------------------------
    # GRAPH 1: Sequential vs Parallel Time Comparison
    # -----------------------------------------------------
    fig, ax = plt.subplots(figsize=(12, 7))
    rects1 = ax.barh(y - height/2, serial_times, height, label='Sequential Time (ms)', color='#e74c3c')
    rects2 = ax.barh(y + height/2, parallel_times, height, label='Parallel Time (ms)', color='#2ecc71')

    ax.set_xlabel('Execution Time (ms)')
    ax.set_title('OpenMP Parallel vs Sequential Processing Time (Averaged Metrics)')
    ax.set_yticks(y)
    ax.set_yticklabels(short_queries)
    ax.legend()
    ax.invert_yaxis()

    plt.tight_layout()
    plt.savefig('results/time_comparison.png', dpi=300)
    print("✅ Saved Graph: results/time_comparison.png")
    
    # -----------------------------------------------------
    # GRAPH 2: Speedup Factor Visualization
    # -----------------------------------------------------
    fig2, ax2 = plt.subplots(figsize=(10, 6))
    
    colors = ['#27ae60' if s >= 2.0 else '#f39c12' for s in speedups]
    bars = ax2.barh(y, speedups, color=colors)
    
    ax2.set_xlabel('Speedup Factor (Multiplier)')
    ax2.set_title('HPC Multi-Core Speedup Factor per Query Type')
    ax2.set_yticks(y)
    ax2.set_yticklabels(short_queries)
    ax2.invert_yaxis()
    
    ax2.axvline(x=8.0, color='red', linestyle='--', alpha=0.5, label='Theoretical Max (8 Cores)')
    ax2.legend()
    
    for bar in bars:
        width = bar.get_width()
        label_x_pos = width + 0.1
        ax2.text(label_x_pos, bar.get_y() + bar.get_height()/2, f"{width:.2f}x", va='center', fontweight='bold')

    plt.tight_layout()
    plt.savefig('results/speedup_analysis.png', dpi=300)
    print("✅ Saved Graph: results/speedup_analysis.png")

if __name__ == "__main__":
    try:
        q, s, p, sp = read_metrics_from_csv()
        if q:
            generate_graphs(q, s, p, sp)
            print("🎉 Graph generation fully completed from native CSV Database! Open the /results folder.")
    except Exception as e:
        print(f"Error parsing CSV metrics: {e}")
