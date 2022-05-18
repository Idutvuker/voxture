import matplotlib.pyplot as plt
import numpy as np

dataset = 'citywall'

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


draw()