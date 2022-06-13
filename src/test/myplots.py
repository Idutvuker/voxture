import matplotlib.pyplot as plt
import numpy as np

dataset = 'saharov'

def load_data(path):
    with open(path, 'r') as fin:
        data = np.array(list(map(float, fin.read().split(',')[:-1])))

    data = np.round(1 / data)

    return data


def draw():
    octree = load_data(f'results/octree_{dataset}_results.csv')
    uv = load_data(f'results/uv_{dataset}_results.csv')

    x = np.arange(len(uv))  # the label locations
    width = 0.42  # the width of the bars

    fig, ax = plt.subplots()
    rects1 = ax.bar(x - width/2, octree, width, label='DAG')
    rects2 = ax.bar(x + width/2, uv, width, label='UV')

    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_xlabel('Viewpoint')
    ax.set_ylabel('AVG FPS')
    ax.set_title(f'Dataset: $\\bf{{{dataset}}}$. Average frames per second from different viewpoints')
    ax.set_xticks(x, x)
    ax.legend()

    ax.bar_label(rects1, padding=3, fontsize=10)
    ax.bar_label(rects2, padding=3, fontsize=10)

    fig.tight_layout()

    # plt.show()
    fig.set_size_inches(12, 6)
    plt.savefig(f'plot_{dataset}.png', dpi=110, bbox_inches='tight')


def pie_chart():
    x = np.arange(2)
    labels = ['Citywall', 'Saharov']

    # tree = [16193, 883]
    # colors = [2024, 158]

    rawT = [16.193, 2.683]
    rawC = [2.024, .335]

    compT = [.883, .156]
    compC = [.158, .028]

    width = 0.3  # the width of the bars

    fig, ax = plt.subplots()
    rects1 = ax.barh(x - width / 2, rawT, width, label='Octree')
    rects2 = ax.barh(x - width / 2, rawC, width, left=rawT, label='Raw colors')

    rects3 = ax.barh(x + width / 2, compT, width, label='DAG')
    rects4 = ax.barh(x + width / 2, compC, width, left=compT, label='Compressed colors (DXT1)')

    ax.bar_label(rects2, padding=3, fmt='%.1f GB')
    ax.bar_label(rects4, padding=3, fmt='%.1f GB')

    ax.set_xlabel('GB')
    ax.set_title(f'Compression rate')

    ax.legend()
    ax.set_xticks(np.arange(21))
    ax.set_yticks(x, labels, size=12)
    ax.set_xlim(right=20)

    fig.tight_layout()

    plt.text(10, 0 + width / 2, '17x compression', verticalalignment='center', ha='center', size=14, weight='bold')
    plt.text(10, 1 + width / 2, '16x compression', verticalalignment='center', ha='center', size=14, weight='bold')

    # plt.show()
    fig.set_size_inches(12, 6)
    plt.savefig(f'plot_compression.png', dpi=110, bbox_inches='tight')

# draw()
pie_chart()
