from __future__ import annotations

import math
import re
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.collections import LineCollection
from matplotlib.colors import Normalize
from matplotlib.patches import Rectangle
from matplotlib.ticker import AutoMinorLocator

ROOT = Path(__file__).resolve().parent
GEO = ROOT / "Fig08a_high_density_stochastic_DFN.geo"
MSH = ROOT / "Fig08a_high_density_stochastic_DFN.msh"
OUT = ROOT / "svg"
PREVIEW = ROOT / "previews"
BLUE = "#0000FF"
MESH = "#A2A8AD"
ENDPOINT = "#5C6670"
INK = "#33475B"
WHITE = "#FFFFFF"

plt.rcParams.update({
    "font.family": "Times New Roman",
    "font.size": 10,
    "axes.labelsize": 12,
    "xtick.labelsize": 10,
    "ytick.labelsize": 10,
    "axes.linewidth": 1.0,
    "axes.unicode_minus": False,
    "svg.fonttype": "none",
    "pdf.fonttype": 42,
    "savefig.dpi": 300,
})


def parse_msh(path):
    lines = path.read_text(encoding="utf-8", errors="ignore").splitlines()
    nodes, elements, names = {}, [], {}
    i = 0
    while i < len(lines):
        t = lines[i].strip()
        if t == "$PhysicalNames":
            n = int(lines[i + 1])
            for row in lines[i + 2:i + 2 + n]:
                dim, tag, name = row.split(" ", 2)
                names[int(tag)] = name.strip().strip('"')
            i += n + 2
        elif t == "$Nodes":
            n = int(lines[i + 1])
            for row in lines[i + 2:i + 2 + n]:
                q = row.split(); nodes[int(q[0])] = (float(q[1]), float(q[2]))
            i += n + 2
        elif t == "$Elements":
            n = int(lines[i + 1])
            for row in lines[i + 2:i + 2 + n]:
                q = [int(v) for v in row.split()]
                nt = q[2]
                elements.append((q[1], q[3] if nt else 0, q[3 + nt:]))
            i += n + 2
        else:
            i += 1
    return nodes, elements, names


def mesh_edges(nodes, elements):
    edges = set()
    for typ, _, conn in elements:
        if typ == 2 and len(conn) >= 3:
            for a, b in ((0, 1), (1, 2), (2, 0)):
                edges.add(tuple(sorted((conn[a], conn[b]))))
    return [[nodes[a], nodes[b]] for a, b in edges]


def parse_geo_lines(path):
    text = path.read_text(encoding="utf-8", errors="ignore")
    points = {}
    for m in re.finditer(r"Point\((\d+)\)\s*=\s*\{\s*([-+0-9.eE]+)\s*,\s*([-+0-9.eE]+)", text):
        points[int(m.group(1))] = (float(m.group(2)), float(m.group(3)))
    lines = {}
    for m in re.finditer(r"Line\((\d+)\)\s*=\s*\{\s*(\d+)\s*,\s*(\d+)\s*\}", text):
        lid, a, b = map(int, m.groups())
        if a in points and b in points:
            lines[lid] = (points[a], points[b])
    physical = {}
    for m in re.finditer(r'Physical\s+Curve\("([^"]+)"\)\s*=\s*\{([^}]*)\}', text, re.S):
        physical[m.group(1)] = [int(v) for v in re.findall(r"-?\d+", m.group(2))]
    return points, lines, physical


def bounds_from_mesh(nodes, elements):
    used = {n for typ, _, conn in elements if typ == 2 for n in conn[:3]}
    xy = np.array([nodes[n] for n in used], float)
    return (xy[:, 0].min(), xy[:, 0].max(), xy[:, 1].min(), xy[:, 1].max())


def style(ax, bounds, tick=2.0):
    xmin, xmax, ymin, ymax = bounds
    ax.set_xlim(xmin, xmax); ax.set_ylim(ymin, ymax)
    ax.set_aspect("equal", adjustable="box")
    ax.set_facecolor(WHITE)
    for s in ax.spines.values():
        s.set_color(INK); s.set_linewidth(1.0)
    ax.set_xlabel("x (m)", labelpad=5); ax.set_ylabel("y (m)", labelpad=5)
    ax.set_xticks(np.arange(math.ceil(xmin / tick) * tick, xmax + 0.1, tick))
    ax.set_yticks(np.arange(math.ceil(ymin / tick) * tick, ymax + 0.1, tick))
    ax.xaxis.set_minor_locator(AutoMinorLocator(2)); ax.yaxis.set_minor_locator(AutoMinorLocator(2))
    ax.tick_params(which="major", direction="in", length=4.2, width=0.9, top=False, right=False, pad=3)
    ax.tick_params(which="minor", direction="in", length=2.5, width=0.7, top=False, right=False)


def save(fig, name):
    OUT.mkdir(exist_ok=True); PREVIEW.mkdir(exist_ok=True)
    svg_path = OUT / f"{name}.svg"
    fig.savefig(svg_path, bbox_inches="tight", pad_inches=0.025, facecolor=WHITE)
    harden_svg_fonts(svg_path)
    fig.savefig(PREVIEW / f"{name}.png", bbox_inches="tight", pad_inches=0.025, facecolor=WHITE, dpi=300)
    plt.close(fig)


def harden_svg_fonts(path):
    """Use explicit SVG font-family/font-size declarations for Office import."""
    text = path.read_text(encoding="utf-8")
    text = re.sub(
        r"font:\s*([0-9.]+)px\s*'Times New Roman';",
        r"font-family: 'Times New Roman'; font-size: \1px;",
        text,
    )
    path.write_text(text, encoding="utf-8")


def original_fractures():
    _, lines, physical = parse_geo_lines(GEO)
    ids = physical.get("random_set_01", [])
    return [lines[i] for i in ids if i in lines]


def geometry_and_mesh():
    nodes, elements, _ = parse_msh(MSH)
    b = bounds_from_mesh(nodes, elements)
    fractures = original_fractures()
    for name, show_mesh, zoom in (("fig08a_geometry", False, None), ("fig08b_mesh", True, None), ("fig08c_mesh_zoom", True, (-1.5, 1.5, -1.5, 1.5))):
        fig, ax = plt.subplots(figsize=(3.85, 3.75))
        ax.add_patch(Rectangle((b[0], b[2]), b[1] - b[0], b[3] - b[2], facecolor=WHITE, edgecolor="none", zorder=0))
        if show_mesh:
            ax.add_collection(LineCollection(mesh_edges(nodes, elements), colors=MESH, linewidths=0.20, zorder=1))
        else:
            ax.add_patch(Rectangle((b[0], b[2]), b[1] - b[0], b[3] - b[2], facecolor="none", edgecolor=INK, linewidth=1.0, zorder=2))
        ax.add_collection(LineCollection(fractures, colors=BLUE, linewidths=0.48 if show_mesh else 0.68, zorder=3))
        ends = np.asarray([p for seg in fractures for p in seg])
        ax.scatter(ends[:, 0], ends[:, 1], s=3.2, color=ENDPOINT, edgecolors="none", zorder=4)
        style(ax, zoom if zoom else b, tick=1.0 if zoom else 2.0)
        if zoom:
            ax.set_xlim(zoom[0], zoom[1]); ax.set_ylim(zoom[2], zoom[3])
        save(fig, name)


def stats_panels():
    fractures = original_fractures()
    p = np.asarray(fractures)
    dx = p[:, 1, 0] - p[:, 0, 0]; dy = p[:, 1, 1] - p[:, 0, 1]
    lengths = np.hypot(dx, dy)
    angles = (np.degrees(np.arctan2(dy, dx)) + 180.0) % 180.0

    # One-set orientation rose, with bar height determined by the observed count.
    fig, ax = plt.subplots(figsize=(3.75, 3.45), subplot_kw={"projection": "polar"})
    edges = np.deg2rad(np.arange(0, 181, 10)); hist, _ = np.histogram(np.deg2rad(angles), bins=edges)
    centers = (edges[:-1] + edges[1:]) / 2
    ax.bar(centers, hist, width=np.diff(edges), color=BLUE, edgecolor=INK, linewidth=0.45, alpha=0.88)
    ax.set_theta_zero_location("E"); ax.set_theta_direction(1); ax.set_thetamin(0); ax.set_thetamax(180)
    ax.set_thetagrids(range(0, 181, 45), fontsize=9); ax.set_yticklabels([]); ax.spines["polar"].set_color(INK)
    ax.grid(color="#B9BEC2", linewidth=0.45, alpha=0.75)
    save(fig, "fig08d_rose")

    for name, values, xlabel, bins in (
        ("fig08e_dip_angle", angles, "Dip angle (°)", np.arange(0, 191, 10)),
        ("fig08f_trace_length", lengths, "Trace length (m)", 18),
    ):
        fig, ax = plt.subplots(figsize=(3.85, 3.35))
        ax.hist(values, bins=bins, density=True, histtype="stepfilled", color=BLUE, alpha=0.82, edgecolor=INK, linewidth=0.55)
        ax.set_xlabel(xlabel, labelpad=5); ax.set_ylabel("Density", labelpad=5)
        ax.set_facecolor(WHITE)
        for s in ax.spines.values(): s.set_color(INK); s.set_linewidth(1.0)
        ax.tick_params(which="both", direction="in", top=False, right=False)
        ax.xaxis.set_minor_locator(AutoMinorLocator(2)); ax.yaxis.set_minor_locator(AutoMinorLocator(2))
        save(fig, name)

    np.savetxt(ROOT / "fig08_single_set_statistics.csv", np.c_[angles, lengths], delimiter=",", header="dip_angle_deg,trace_length_m", comments="")
    print(f"fractures={len(fractures)} length_range=({lengths.min():.4f},{lengths.max():.4f}) angle_range=({angles.min():.2f},{angles.max():.2f})")


if __name__ == "__main__":
    geometry_and_mesh(); stats_panels()
