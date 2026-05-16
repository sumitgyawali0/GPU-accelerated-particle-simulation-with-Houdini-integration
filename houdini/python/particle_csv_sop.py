# Paste this ENTIRE file into the Python SOP script editor.
#
# Edit DEFAULT_EXPORT_DIR to your export folder (use forward slashes).
#
# IMPORTANT: Export enough frames from particle_sim, e.g.:
#   particle_sim.exe --frames 48 --export-dir export
# Then set Houdini playbar to 0–48 (or 1–48) to match.

import csv
import glob
import os
import re
from pathlib import Path

import hou

DEFAULT_EXPORT_DIR = "B:/blogs/CUDA_coursework/export"

node = hou.pwd()
geo = node.geometry()
geo.clear()

export_dir = DEFAULT_EXPORT_DIR
export_parm = node.parm("export_dir")
if export_parm is not None:
    export_dir = hou.expandString(export_parm.evalAsString())

export_dir = Path(export_dir.strip()).as_posix()

# Discover exported frames: particles_0000.csv, particles_0001.csv, ...
pattern = os.path.join(export_dir, "particles_*.csv")
available_frames = []
for path in glob.glob(pattern):
    match = re.search(r"particles_(\d+)\.csv$", Path(path).as_posix())
    if match:
        available_frames.append(int(match.group(1)))

if not available_frames:
    raise hou.NodeError(
        "No CSV files found in:\n  {}\n\n"
        "Run particle_sim with --export-dir and --export-format csv first.".format(
            export_dir
        )
    )

available_frames.sort()
min_frame = available_frames[0]
max_frame = available_frames[-1]

timeline_frame = int(hou.frame())
load_frame = timeline_frame

def set_node_warning(message):
    try:
        node.setWarning(message)
    except AttributeError:
        pass

if load_frame not in available_frames:
    if load_frame > max_frame:
        load_frame = max_frame
        set_node_warning(
            "Timeline frame {} > last export ({}). Holding frame {}.".format(
                timeline_frame, max_frame, max_frame
            )
        )
    elif load_frame < min_frame:
        load_frame = min_frame
        set_node_warning(
            "Timeline frame {} < first export ({}). Holding frame {}.".format(
                timeline_frame, min_frame, min_frame
            )
        )
    else:
        lower = [f for f in available_frames if f <= load_frame]
        load_frame = lower[-1] if lower else min_frame
        set_node_warning(
            "Frame {} not exported; using nearest frame {}.".format(
                timeline_frame, load_frame
            )
        )
else:
    set_node_warning("")

filename = "{}/particles_{:04d}.csv".format(export_dir, load_frame)

with open(filename, newline="") as csv_file:
    rows = list(csv.DictReader(csv_file))

if rows:
    geo.addAttrib(hou.attribType.Point, "v", hou.Vector3(0.0, 0.0, 0.0))
    geo.addAttrib(hou.attribType.Point, "Cd", hou.Vector3(1.0, 1.0, 1.0))
    geo.addAttrib(hou.attribType.Point, "mass", 0.0)

    for row in rows:
        pt = geo.createPoint()

        px = float(row["px"])
        py = float(row["py"])
        pz = float(row["pz"])
        vx = float(row["vx"])
        vy = float(row["vy"])
        vz = float(row["vz"])
        cr = float(row["Cd_r"])
        cg = float(row["Cd_g"])
        cb = float(row["Cd_b"])
        mass = float(row["mass"])

        pt.setPosition(hou.Vector3(px, py, pz))
        pt.setAttribValue("v", hou.Vector3(vx, vy, vz))
        pt.setAttribValue("Cd", hou.Vector3(cr, cg, cb))
        pt.setAttribValue("mass", mass)
