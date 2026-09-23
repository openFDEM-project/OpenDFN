# Input Format

OpenDFN reads a plain-text `.dfn` deck. Commands use the `odfn.geometry.*`
prefix. Blank lines and lines beginning with `#` are ignored.

## Domain and meshing

```text
odfn.geometry.domain [xmin xmax ymin ymax]
odfn.geometry.square 'rock' [xmin xmax ymin ymax]
odfn.geometry.minsize value
odfn.geometry.minangle value
odfn.geometry.iteration integer
odfn.geometry.mesh.size 'default' value
odfn.geometry.mesh delaunay
```

`domain` is the generation box and `square` defines the host surface. The
minimum feature size and minimum angle are geometric screening thresholds.
The mesh-size value is the target element size in model units.

## Deterministic joints

```text
odfn.geometry.cut.joint 'name' 'host' [x1 y1 x2 y2]
```

## Continuous joint sets

```text
odfn.geometry.cut.jset 'name' 'host' dip angle space spacing start x y
```

Omitting `trace` and `gap` produces continuous traces across the host.

## Discontinuous joint sets

Constant or normally distributed spacing, trace, and gap values are supported:

```text
odfn.geometry.cut.jset 'name' 'host' dip angle \
  n_space mean sd n_trace mean sd n_gap mean sd start x y
```

The `n_` prefix selects a normal distribution. Constant values use the
unprefixed keyword.

## Stochastic DFNs

```text
odfn.geometry.cut.dfn 'name' 'host' count integer \
  u_dip lower upper n_length mean sd
```

The available orientation and length keywords are documented by the example
decks under `examples/`.

## Coordinate-derived networks

```text
odfn.geometry.import.rdfn scale 'name' 'host' 'coordinates.txt'
```

Each non-empty coordinate row defines one polyline. The scale is the number of
input coordinate units per model length unit.
