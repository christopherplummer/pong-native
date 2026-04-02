#import <Foundation/Foundation.h>

#include "../common/pong.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        struct PongState state;
        pong_init(&state);

        /*
         * Scaffold only.
         *
         * A future macOS implementation can create an AppKit window,
         * collect input events, call the shared assembly logic, and render
         * from the shared state every frame.
         *
         * Likely future options:
         * - AppKit + Metal or OpenGL
         * - SDL2 on macOS
         */

        NSLog(@"macOS scaffold for pong-native.");
        NSLog(@"Shared core initialized: %d x %d, score %d-%d",
              state.screen_width,
              state.screen_height,
              state.player1_score,
              state.player2_score);
    }

    return 0;
}
