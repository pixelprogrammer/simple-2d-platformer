const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Create executable
    const exe = b.addExecutable(.{
        .name = "simple-2d-platformer",
        .target = target,
        .optimize = optimize,
    });

    const defaultFlags = .{
        "-std=c99",
        "-Wall",
        "-Wextra",
    };
    const debugFlags = .{
        "-g",
        "-O0",
        "-DDEBUG",
    };
    const releaseFlags = .{
        "-O3",
        "-DNDEBUG",
        "-DRELEASE",
    };

    const flags = if (optimize == .Debug) defaultFlags ++ debugFlags else defaultFlags ++ releaseFlags;

    // Add C files
    exe.addCSourceFiles(.{
        .files = &.{
            "src/main.c",
            "src/animation.c",
            "src/enemy.c",
            "src/healthbar.c",
            "src/platform.c",
            "src/player.c",
            "src/screen.c",
            "src/weapons.c",
        },
        .flags = &flags,
    });

    // Include src directory
    exe.addIncludePath(b.path("src"));

    // Link raylib
    exe.linkSystemLibrary("raylib");
    exe.linkLibC();

    // Platform-specific libraries
    switch (target.result.os.tag) {
        .linux => {
            exe.linkSystemLibrary("m");
            exe.linkSystemLibrary("pthread");
            exe.linkSystemLibrary("dl");
            exe.linkSystemLibrary("rt");
        },
        .macos => {
            exe.linkFramework("CoreVideo");
            exe.linkFramework("IOKit");
            exe.linkFramework("Cocoa");
            exe.linkFramework("GLUT");
            exe.linkFramework("OpenGL");
        },
        .windows => {
            exe.linkSystemLibrary("winmm");
        },
        else => {},
    }

    // Install the executable
    b.installArtifact(exe);

    // Create run step
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the game");
    run_step.dependOn(&run_cmd.step);
}

