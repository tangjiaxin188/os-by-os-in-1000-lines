#!/usr/bin/env python3
"""
ASCII Art Generator
使用指定的 TTF 字体将字符串渲染为 ASCII 艺术字。

用法:
    python ascii_art.py <字体文件.ttf> <字符串> [选项]

示例:
    python ascii_art.py Arial.ttf "Hello"
    python ascii_art.py font.ttf "Hi" --size 30 --output out.txt
"""

import argparse
import sys
from pathlib import Path

try:
    from PIL import Image, ImageDraw, ImageFont
except ImportError:
    print("错误: 需要安装 Pillow 库。请运行: pip install Pillow", file=sys.stderr)
    sys.exit(1)


# 默认 ASCII 字符集，从暗到亮（像素值越小 -> 越暗 -> 越靠前）
DEFAULT_CHARSET = " .:-=+*#%@"
# 更细腻的字符集（可选）
# DEFAULT_CHARSET = " .'`^\",:;Il!i><~+_-?][}{1)(|/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"


def load_font(font_path: str, size: int) -> ImageFont.FreeTypeFont:
    """加载 TTF 字体文件。"""
    path = Path(font_path)
    if not path.exists():
        raise FileNotFoundError(f"字体文件不存在: {font_path}")
    if path.suffix.lower() not in (".ttf", ".otf"):
        print(f"警告: '{font_path}' 不是标准的 TTF/OTF 字体文件", file=sys.stderr)
    try:
        return ImageFont.truetype(str(path), size)
    except OSError as e:
        raise OSError(f"无法加载字体 '{font_path}': {e}")


def char_to_ascii(
    char: str,
    font: ImageFont.FreeTypeFont,
    charset: str,
    aspect_ratio: float = 2.0,
) -> list[str]:
    """
    将单个字符渲染为 ASCII 艺术字，返回行列表。

    参数:
        char: 要渲染的字符
        font: PIL 字体对象
        charset: ASCII 字符集（从暗到亮）
        aspect_ratio: 终端字符的 高/宽 比例，用于垂直压缩（默认 2.0）
    """
    if char == "\n":
        return [""]

    # 1. 测量字符尺寸
    # 使用 getbbox 获取字符的实际边界
    dummy = Image.new("L", (1, 1))
    draw = ImageDraw.Draw(dummy)
    try:
        # Pillow >= 10.0
        bbox = draw.textbbox((0, 0), char, font=font)
        width = bbox[2] - bbox[0]
        height = bbox[3] - bbox[1]
        offset_x, offset_y = bbox[0], bbox[1]
    except AttributeError:
        # 兼容旧版本
        width, height = draw.textsize(char, font=font)
        offset_x, offset_y = 0, 0

    if width <= 0 or height <= 0:
        # 空白字符（如空格）
        return [""]

    # 2. 创建图像并绘制字符
    img = Image.new("L", (width + 2, height + 2), color=255)  # 白底
    draw = ImageDraw.Draw(img)
    draw.text((-offset_x + 1, -offset_y + 1), char, fill=0, font=font)  # 黑字

    # 3. 根据 aspect_ratio 对图像进行垂直采样
    # 终端字符通常是"瘦高"的，所以需要把多行像素压缩成一行
    target_height = max(1, int(height / aspect_ratio))
    target_width = width

    # 使用 resize 进行下采样（LANCZOS 抗锯齿）
    img_resized = img.resize((target_width, target_height), Image.LANCZOS)

    # 4. 将像素值映射为 ASCII 字符
    pixels = img_resized.load()
    lines = []
    n = len(charset)
    for y in range(target_height):
        row_chars = []
        for x in range(target_width):
            # 像素值 0(黑) -> 255(白)
            # 映射到 charset：黑 -> charset[0]，白 -> charset[-1]
            pixel = pixels[x, y]
            idx = int((255 - pixel) / 256 * n)
            idx = max(0, min(n - 1, idx))
            row_chars.append(charset[idx])
        # 去除行尾多余空格
        lines.append("".join(row_chars).rstrip())

    return lines


def string_to_ascii(
    text: str,
    font_path: str,
    size: int = 20,
    charset: str = DEFAULT_CHARSET,
    aspect_ratio: float = 2.2,
    spacing: int = 1,
) -> str:
    """
    将整个字符串转换为 ASCII 艺术字。
    每个字符的 ASCII 艺术水平拼接,换行符按行处理。
    
    参数:
        spacing: 字符之间的空格数（默认: 1）
    """
    font = load_font(font_path, size)
    separator = " " * spacing  # 字符间隔

    # 按行分割输入文本
    input_lines = text.split("\n")
    result_lines = []

    for input_line in input_lines:
        # 渲染这一行中的每个字符
        char_blocks = [char_to_ascii(c, font, charset, aspect_ratio) for c in input_line]

        if not char_blocks:
            result_lines.append("")
            continue

        # 找出这一行中所有字符的最大高度
        max_height = max((len(block) for block in char_blocks), default=0)

        # 水平拼接每个字符的每一行
        for y in range(max_height):
            row_parts = []
            for block in char_blocks:
                if y < len(block):
                    row_parts.append(block[y])
                else:
                    # 高度不足的部分用空格填充（保持对齐）
                    # 使用第一个非空行的高度作为参考宽度
                    ref_width = len(block[0]) if block else 0
                    row_parts.append(" " * ref_width)
            # 使用 separator 连接各个字符块
            result_lines.append(separator.join(row_parts).rstrip())

        # 每行输入之间留一行空行，分隔更清晰
        result_lines.append("")

    return "\n".join(result_lines)


def parse_args():
    parser = argparse.ArgumentParser(
        description="使用指定的 TTF 字体将字符串渲染为 ASCII 艺术字",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  %(prog)s font.ttf "Hello"
  %(prog)s font.ttf "Hi" --size 30
  %(prog)s font.ttf "World" --charset " .oO@" --output out.txt
  %(prog)s font.ttf "Test" --ratio 1.8
        """,
    )
    parser.add_argument("font", help="TTF/OTF 字体文件路径")
    parser.add_argument("text", help="要转换的字符串")
    parser.add_argument(
        "-s", "--size", type=int, default=20, help="字体大小（默认: 20）"
    )
    parser.add_argument(
        "-c",
        "--charset",
        default=DEFAULT_CHARSET,
        # 修复：将 % 转义为 %%，避免 argparse 误解析
        help=f"ASCII 字符集，从暗到亮（默认: '{DEFAULT_CHARSET.replace('%', '%%')}'）",
    )
    parser.add_argument(
        "-r",
        "--ratio",
        type=float,
        default=2.2,
        help="终端字符高宽比，用于垂直压缩（默认: 2.2）",
    )
    parser.add_argument(
        "-p",
        "--spacing",
        type=int,
        default=1,
        help="字符之间的空格数（默认: 1）",
    )
    parser.add_argument(
        "-o", "--output", help="输出到文件（不指定则打印到终端）"
    )
    return parser.parse_args()


def main():
    args = parse_args()

    try:
        result = string_to_ascii(
            text=args.text,
            font_path=args.font,
            size=args.size,
            charset=args.charset,
            aspect_ratio=args.ratio,
            spacing=args.spacing,  # 添加这一行
        )
    except (FileNotFoundError, OSError) as e:
        print(f"错误: {e}", file=sys.stderr)
        sys.exit(1)

    if args.output:
        Path(args.output).write_text(result, encoding="utf-8")
        print(f"✓ 已保存到: {args.output}", file=sys.stderr)
    else:
        print(result)


if __name__ == "__main__":
    main()