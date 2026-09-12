# OpenDFN Examples

所有案例均可使用以下形式运行：

```powershell
src\bin\Release\opendfn.exe -in examples\<case>\<case>.dfn
```

每次运行会在案例目录同时生成同名的 `.msh`、`.geo` 和 `.vtk` 文件。

批量生成并校验全部案例：

```powershell
powershell -ExecutionPolicy Bypass -File examples\generate_all_formats.ps1
```

每个案例目录还包含一个独立的 `generate_formats.ps1`，例如：

```powershell
powershell -ExecutionPolicy Bypass -File examples\input_real_dfn\generate_formats.ps1
```

| 案例目录 | 功能 | 主要关键字 |
|---|---|---|
| `single_joint` | 单条确定性节理 | `cut.joint` |
| `multiple_joints` | 多条不同位置与方向的确定性节理 | 多个 `cut.joint` |
| `continuous_joint_sets` | 两组贯穿岩样的连续节理组 | `cut.jset dip space start` |
| `discontinuous_joint_sets` | 两组带迹长和间断长度的断续节理组 | `cut.jset n_trace n_gap` |
| `arbitrary_dfn` | 倾角在 0-180 度均匀分布的任意随机 DFN | `cut.dfn u_dip n_length` |
| `input_real_dfn` | 从折线坐标文本导入真实 DFN | `import.rdfn` |
| `basic_geometry` | 基础矩形与交叉显式节理 | `square`, `cut.joint` |
| `random_dfn` | 两组统计参数不同的随机 DFN | 两个 `cut.dfn` |
| `realistic_dfn` | 原始坐标驱动 rDFN 示例 | `import.rdfn` |

连续节理组不设置 `trace`/`gap`；只要设置其中任意一项，就会进入断续节理生成分支。随机参数前缀 `n_` 表示正态分布，`u_dip` 表示均匀倾角分布。

## 输出格式

| 扩展名 | 内容 | 典型用途 |
|---|---|---|
| `.msh` | Gmsh 2.0 ASCII 网格，保留物理分组 | 数值计算、求解器输入 |
| `.geo` | Gmsh 展开后的可重放几何脚本 | 几何检查、修改和重新划分网格 |
| `.vtk` | Legacy VTK 非结构网格 | ParaView、VisIt 等后处理可视化 |
