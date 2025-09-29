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
            "src/fallable.c",
            "src/jumpable.c",
            "src/healthbar.c",
            "src/platform.c",
            "src/player.c",
            "src/sprite.c",
            "src/sound.c",
            "src/stunnable.c",
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

    // Create test executable
    const test_exe = b.addExecutable(.{
        .name = "test_player_jump",
        .target = target,
        .optimize = optimize,
    });

    // Add test C files
    test_exe.addCSourceFiles(.{
        .files = &.{
            "tests/test_player_jump.c",
            "src/animation.c",
            "src/fallable.c",
            "src/jumpable.c",
            "src/healthbar.c",
            "src/platform.c",
            "src/player.c",
            "src/sprite.c",
            "src/sound.c",
            "src/stunnable.c",
            "src/weapons.c",
        },
        .flags = &flags,
    });

    // Include src directory for tests
    test_exe.addIncludePath(b.path("src"));

    // Link raylib for tests
    test_exe.linkSystemLibrary("raylib");
    test_exe.linkLibC();

    // Platform-specific libraries for tests
    switch (target.result.os.tag) {
        .linux => {
            test_exe.linkSystemLibrary("m");
            test_exe.linkSystemLibrary("pthread");
            test_exe.linkSystemLibrary("dl");
            test_exe.linkSystemLibrary("rt");
        },
        .macos => {
            test_exe.linkFramework("CoreVideo");
            test_exe.linkFramework("IOKit");
            test_exe.linkFramework("Cocoa");
            test_exe.linkFramework("GLUT");
            test_exe.linkFramework("OpenGL");
        },
        .windows => {
            test_exe.linkSystemLibrary("winmm");
        },
        else => {},
    }

    // Install the test executable
    b.installArtifact(test_exe);

    // Create test run step
    const test_run_cmd = b.addRunArtifact(test_exe);
    test_run_cmd.step.dependOn(b.getInstallStep());

    const test_step = b.step("test", "Run jump mechanics tests");
    test_step.dependOn(&test_run_cmd.step);
}
