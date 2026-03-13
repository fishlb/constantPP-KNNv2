"""
draw Fig.3 and Fig.4 of the paper
"""

import matplotlib
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pandas as pd
import numpy as np
import re

matplotlib.rcParams.update({
    "font.family": "serif",
    "mathtext.fontset": "dejavuserif",
    "axes.labelsize": 10,
    "legend.fontsize": 8.5,
    "xtick.labelsize": 8.5,
    "ytick.labelsize": 8.5,
    "axes.linewidth": 0.8,
    "axes.facecolor": "white",
    "figure.facecolor": "white",
    "axes.grid": True,
    "grid.color": "#bbbbbb",
    "grid.linestyle": "--",
    "grid.linewidth": 0.6,
})

# ── Parser ───────────────────────────────────────────────────────────────────
def parse_section(text, section_header):
    pattern = re.escape(section_header) + r".*?\n(.*?)(?=\n---|\Z)"
    m = re.search(pattern, text, re.DOTALL)
    if not m:
        raise ValueError(f"Section '{section_header}' not found.")
    block = m.group(1).strip()
    lines = [l for l in block.splitlines() if l.strip()]
    col_names_raw = lines[0][len("Scheme"):].split()

    def rebuild_names(tokens):
        names, i = [], 0
        while i < len(tokens):
            if i + 1 < len(tokens) and tokens[i + 1].startswith("["):
                names.append(f"{tokens[i]} {tokens[i+1]}")
                i += 2
            else:
                names.append(tokens[i])
                i += 1
        return names

    col_names = rebuild_names(col_names_raw)
    idx_name  = lines[1].split()[0]
    records   = {}
    for line in lines[2:]:
        tokens = line.split()
        records[tokens[0]] = [float(tokens[i]) for i in range(1, len(tokens), 2)]

    df = pd.DataFrame.from_dict(records, orient="index", columns=col_names)
    df.index.name = idx_name
    df.index = pd.to_numeric(df.index)
    return df

# ── Load data ────────────────────────────────────────────────────────────────
with open("Fig3_OfflineCost_Summary.txt", encoding="utf-8") as f:
    fig3_text = f.read()
with open("Fig4_LatencyResult_Summary.txt", encoding="utf-8") as f:
    fig4_text = f.read()

df3a = parse_section(fig3_text, "--- Fig.3a: Varying dataset size n (Fixed k=5) ---")
df3b = parse_section(fig3_text, "--- Fig.3b: Varying neighbor size k (Fixed n=400) ---")
df4a = parse_section(fig4_text, "--- Fig.4a: Varying dataset size n (Fixed k=5) ---")
df4b = parse_section(fig4_text, "--- Fig.4b: Varying neighbor size k (Fixed n=400) ---")

# ── Scheme styles ─────────────────────────────────────────────────────────────
FIG3_SCHEMES = [
    ("Offline(Sequential)", "Offline (Sequential)", "#cc44aa", "--", "x"),
    ("Offline(Parallel)",   "Offline (Parallel)",   "#cc44aa", "-",  "x"),
]
FIG4_SCHEMES = [
    ("Scheme [11]",      "[11]",             "#33aa33", "-",  "o"),
    ("Scheme [13]",      "[13]",             "#3355cc", "-",  "^"),
    ("Scheme [12]",      "[12]",             "#cc3333", "-",  "s"),
    ("Ours(Sequential)", "Ours(Sequential)", "#cc44aa", "--", "x"),
    ("Ours(Parallel)",   "Ours(Parallel)",   "#cc44aa", "-",  "x"),
]

# ── Auto break-range detector ────────────────────────────────────────────────
def auto_break_range(df, pad_lo=0.15, pad_hi=0.18, min_gap_ratio=1.5):
    """
    Automatically detect the best y-axis break point from the data.

    Strategy: find the largest RELATIVE gap between consecutive sorted values.
      rel_gap = (v_next - v_cur) / v_cur
    If rel_gap >= min_gap_ratio, use that break point.
    Fallback: use the largest absolute gap (always returns valid ylims, never None).

    Returns (ylim_lo, ylim_hi, top_frac) — guaranteed non-None.
    """
    vals = np.sort(df.values.flatten())
    vals = vals[vals > 0]
    if len(vals) < 2:
        vmax = float(df.values.max())
        return (0, vmax * 1.1), (vmax * 0.5, vmax * 1.3), 0.55

    gaps = np.diff(vals)
    rel  = gaps / vals[:-1]

    idx = int(np.argmax(rel))
    if rel[idx] >= min_gap_ratio:
        best_idx = idx
    else:
        best_idx = int(np.argmax(gaps))

    gap_bot = float(vals[best_idx])
    gap_top = float(vals[best_idx + 1])
    vmax    = float(vals[-1])

    ylim_lo = (0,              gap_bot * (1 + pad_lo))
    ylim_hi = (gap_top * 0.97, vmax    * (1 + pad_hi))

    lo_span  = ylim_lo[1]
    hi_span  = ylim_hi[1] - ylim_hi[0]
    top_frac = float(np.clip(hi_span / (lo_span + hi_span), 0.40, 0.72))

    return ylim_lo, ylim_hi, top_frac


def nice_step(vmin, vmax, n_ticks=5):
    """Return a rounded tick step for the given range."""
    import numpy as np
    span = vmax - vmin
    if span <= 0:
        return 1
    raw = span / n_ticks
    mag = 10 ** np.floor(np.log10(raw + 1e-12))
    for f in [1, 2, 2.5, 5, 10]:
        if mag * f >= raw:
            return mag * f
    return mag * 10


# ── Broken-axis builder ───────────────────────────────────────────────────────
def make_broken_pair(fig, rect, ylim_hi, ylim_lo, top_frac=0.55):
    """
    rect = [left, bottom, width, height] in figure-fraction coords.
    Returns (ax_hi, ax_lo): two stacked Axes simulating a broken y-axis.
    """
    l, b, w, h = rect
    h_hi = h * top_frac
    h_lo = h * (1 - top_frac)

    ax_hi = fig.add_axes([l, b + h_lo, w, h_hi])
    ax_lo = fig.add_axes([l, b,        w, h_lo], sharex=ax_hi)

    ax_hi.set_ylim(*ylim_hi)
    ax_lo.set_ylim(*ylim_lo)

    # Hide inner-facing spines
    ax_hi.spines["bottom"].set_visible(False)
    ax_lo.spines["top"].set_visible(False)
    ax_hi.tick_params(axis="x", which="both", bottom=False, labelbottom=False)

    # Hide outer redundant spines
    for ax in (ax_hi, ax_lo):
        ax.spines["top"].set_visible(False)
        ax.spines["right"].set_visible(False)

    # Diagonal break marks at join line
    d = 0.015
    bkw = dict(color="k", clip_on=False, linewidth=1.0, zorder=10)
    for xp in (0, 1):
        ax_hi.plot((xp - d, xp + d), (-d,  d),  transform=ax_hi.transAxes, **bkw)
        ax_lo.plot((xp - d, xp + d), (1-d, 1+d), transform=ax_lo.transAxes, **bkw)

    return ax_hi, ax_lo


def plot_on_pair(ax_hi, ax_lo, df, schemes):
    """Plot each scheme on both axes; return legend handles (from ax_lo)."""
    handles = []
    for col, label, color, ls, mk in schemes:
        if col not in df.columns:
            continue
        kw = dict(color=color, linestyle=ls, marker=mk,
                  linewidth=1.5, markersize=5,
                  markerfacecolor="none", markeredgewidth=1.4)
        ax_hi.plot(df.index, df[col], **kw)
        h, = ax_lo.plot(df.index, df[col], label=label, **kw)
        handles.append(h)
    return handles



def add_broken_ylabel(fig, ax_hi, ax_lo, text="Running Time (s)", fontsize=10):
    """Place a y-axis label centred across both broken-axis panels."""
    fig.canvas.draw()   # ensure positions are finalised
    pos_hi = ax_hi.get_position()
    pos_lo = ax_lo.get_position()
    # vertical centre between bottom of ax_lo and top of ax_hi
    y_mid = (pos_lo.y0 + pos_hi.y0 + pos_hi.height) / 2
    # x position: slightly left of the axes left edge
    x_pos = pos_lo.x0 - 0.045
    fig.text(x_pos, y_mid, text,
             ha="center", va="center", fontsize=fontsize,
             rotation=90, transform=fig.transFigure)

def style_pair(ax_hi, ax_lo, xlabel, xticks, ylim_hi=None, ylim_lo=None,
               ytick_hi=None, ytick_lo=None, legend_handles=None):
    ax_lo.set_xlabel(xlabel, fontsize=10)
    ax_hi.set_ylabel("")
    ax_lo.set_ylabel("")
    # ylabel is placed externally via add_broken_ylabel() after fig.canvas.draw()
    ax_lo.set_xticks(xticks)
    # Re-apply ylims explicitly after set_xticks (which can trigger autoscale)
    if ylim_hi: ax_hi.set_ylim(*ylim_hi)
    if ylim_lo: ax_lo.set_ylim(*ylim_lo)
    # Auto tick step
    if ytick_hi:
        ax_hi.yaxis.set_major_locator(ticker.MultipleLocator(ytick_hi))
    else:
        ax_hi.yaxis.set_major_locator(
            ticker.MultipleLocator(nice_step(*ax_hi.get_ylim())))
    if ytick_lo:
        ax_lo.yaxis.set_major_locator(ticker.MultipleLocator(ytick_lo))
    else:
        ax_lo.yaxis.set_major_locator(
            ticker.MultipleLocator(nice_step(*ax_lo.get_ylim())))
    if legend_handles:
        ax_hi.legend(handles=legend_handles, loc="upper left",
                     frameon=True, framealpha=0.9,
                     edgecolor="#cccccc", fontsize=8.5)

# ── Figure layout ─────────────────────────────────────────────────────────────
fig = plt.figure(figsize=(12, 10))
fig.patch.set_facecolor("white")

COL_W  = 0.38
COL_L  = [0.10, 0.57]
ROW_H  = 0.33
ROW_B  = [0.57, 0.13]

# ---------- Fig.3a  (normal axis, no break) ----------
ax3a = fig.add_axes([COL_L[0], ROW_B[0], COL_W, ROW_H])
for col, label, color, ls, mk in FIG3_SCHEMES:
    if col in df3a.columns:
        ax3a.plot(df3a.index, df3a[col],
                  label=label, color=color, linestyle=ls, marker=mk,
                  linewidth=1.5, markersize=5,
                  markerfacecolor="none", markeredgewidth=1.4)
ax3a.set_xlabel("Number of Samples(n)", fontsize=10)
ax3a.set_ylabel("Running Time (s)", fontsize=10)
ax3a.set_xticks(df3a.index)
ax3a.set_ylim(bottom=0)
ax3a.spines["top"].set_visible(False)
ax3a.spines["right"].set_visible(False)
ax3a.legend(loc="upper left", frameon=True, framealpha=0.9,
            edgecolor="#cccccc", fontsize=8.5)
ax3a_lo = ax3a  # alias for caption code below

# ---------- Fig.3b  (auto broken axis, upper ceiling ≥ 20) ----------
_lo3b, _hi3b, _tf3b = auto_break_range(df3b, pad_hi=1.10)
_hi3b = (_hi3b[0], max(_hi3b[1], 20.0))   # ensure upper ceiling ≥ 20
ax3b_hi, ax3b_lo = make_broken_pair(
    fig, [COL_L[1], ROW_B[0], COL_W, ROW_H],
    ylim_hi=_hi3b, ylim_lo=_lo3b, top_frac=_tf3b)
h3b = plot_on_pair(ax3b_hi, ax3b_lo, df3b, FIG3_SCHEMES)
style_pair(ax3b_hi, ax3b_lo, "Value of k", df3b.index,
           ylim_hi=_hi3b, ylim_lo=_lo3b, legend_handles=h3b)
add_broken_ylabel(fig, ax3b_hi, ax3b_lo)

# ---------- Fig.4a  (auto broken axis) ----------
_lo4a, _hi4a, _tf4a = auto_break_range(df4a)
ax4a_hi, ax4a_lo = make_broken_pair(
    fig, [COL_L[0], ROW_B[1], COL_W, ROW_H],
    ylim_hi=_hi4a, ylim_lo=_lo4a, top_frac=_tf4a)
h4a = plot_on_pair(ax4a_hi, ax4a_lo, df4a, FIG4_SCHEMES)
style_pair(ax4a_hi, ax4a_lo, "Number of Samples(n)", df4a.index,
           ylim_hi=_hi4a, ylim_lo=_lo4a, legend_handles=h4a)
add_broken_ylabel(fig, ax4a_hi, ax4a_lo)

# ---------- Fig.4b  (auto broken axis) ----------
_lo4b, _hi4b, _tf4b = auto_break_range(df4b)
ax4b_hi, ax4b_lo = make_broken_pair(
    fig, [COL_L[1], ROW_B[1], COL_W, ROW_H],
    ylim_hi=_hi4b, ylim_lo=_lo4b, top_frac=_tf4b)
h4b = plot_on_pair(ax4b_hi, ax4b_lo, df4b, FIG4_SCHEMES)
style_pair(ax4b_hi, ax4b_lo, "Value of k", df4b.index,
           ylim_hi=_hi4b, ylim_lo=_lo4b, legend_handles=h4b)
add_broken_ylabel(fig, ax4b_hi, ax4b_lo)

# ── Sub-captions & figure captions ───────────────────────────────────────────
SUB_GAP = 0.055
FIG_GAP = 0.038

kw_sub = dict(ha="center", va="top", fontsize=9,  fontfamily="serif",
              transform=fig.transFigure)
kw_fig = dict(ha="center", va="top", fontsize=10, fontfamily="serif",
              transform=fig.transFigure)

# Fig.3 sub-captions
fig.text(COL_L[0] + COL_W/2, ROW_B[0] - SUB_GAP,
         "(a) Offline computation cost under different dataset sizes.", **kw_sub)
fig.text(COL_L[1] + COL_W/2, ROW_B[0] - SUB_GAP,
         r"(b) Offline computation cost under different $k$.", **kw_sub)
fig.text(0.50, ROW_B[0] - SUB_GAP - FIG_GAP,
         "Fig. 3:  Offline computation cost of our scheme.", **kw_fig)

# Fig.4 sub-captions
fig.text(COL_L[0] + COL_W/2, ROW_B[1] - SUB_GAP,
         "(a) Computation cost under different dataset sizes.", **kw_sub)
fig.text(COL_L[1] + COL_W/2, ROW_B[1] - SUB_GAP,
         r"(b) Computation cost under different $k$.", **kw_sub)
fig.text(0.50, ROW_B[1] - SUB_GAP - FIG_GAP,
         "Fig. 4:  Comparison of computation cost between different schemes.", **kw_fig)

# ── Save & show ───────────────────────────────────────────────────────────────
out = "Fig3_and_Fig4.png"
plt.savefig(out, dpi=180, bbox_inches="tight")
plt.show()
print(f"Saved -> {out}")