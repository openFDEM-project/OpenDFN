# Mesh Quality

OpenDFN reports quality metrics from the generated MSH2 node coordinates and
triangle connectivity. The minimum interior angle is the smallest corner angle
of each triangle. The normalized quality measure is

```text
qc = 4 sqrt(3) A / (l1^2 + l2^2 + l3^2)
```

where `A` is the triangle area and `l1`, `l2`, and `l3` are the edge lengths.
An equilateral triangle has `qc = 1`; `qc` approaches zero for a degenerate
triangle.

## Required checks

For each release case, report:

- node, triangle, and line-element counts;
- minimum, mean, median, and lower quantiles of the minimum angle and `qc`;
- counts and percentages below 1, 5, 10, and 20 degrees;
- counts and percentages with `qc < 0.01`;
- zero-area, negative-area, duplicate-node, and nonconforming-edge checks.

Near-degenerate elements must not be hidden by average values. Very small
angles can reduce numerical stability, increase condition numbers, and affect
contact, stiffness, or flow calculations in downstream solvers. A downstream
workflow should therefore define a quality threshold and either reject,
locally remesh, or explicitly flag elements that fail it.

The mapped cases in the SoftwareX validation contain a small tail of very
small-angle elements. The reported mean values describe the bulk mesh but do
not establish that every element is suitable for every downstream numerical
method.
