# Header layout

OpenDFN currently keeps implementation headers next to their source modules
under `src/` because the parser and geometry modules include one another by
module-relative paths such as `common/...` and `geometry/...`. This directory
is reserved for a future stable public API; no headers are duplicated here to
avoid two competing copies of the same interface.
