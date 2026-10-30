/*
 * ============================================================================
 * Course Outcome 2: Process Management
 * Program: process_tree.c
 * Purpose: Demonstrates a 3-tier process tree (Grandparent -> Parent -> Child),
 *          visualizing hierarchical process management in Linux.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("       CO2: Hierarchical Process Tree & Lifecycle Demo         \n");
    printf("===============================================================\n\n");

    printf("\033[1;34m[Tier 1: Root / Grandparent]\033[0m PID %d started\n", getpid());
    pid_t p1 = fork();

    if (p1 < 0) {
        perror("fork 1 failed");
        exit(1);
    }

    if (p1 == 0) {
        // Tier 2: Parent
        printf("  \033[1;36m[Tier 2: Intermediate Child]\033[0m PID %d (Parent %d) created\n", getpid(), getppid());
        pid_t p2 = fork();

        if (p2 < 0) {
            perror("fork 2 failed");
            exit(1);
        }

        if (p2 == 0) {
            // Tier 3: Leaf / Grandchild
            printf("    \033[1;32m[Tier 3: Leaf Grandchild]\033[0m PID %d (Parent %d) running!\n", getpid(), getppid());
            printf("    Work completed at Tier 3. Exiting...\n");
            exit(103);
        } else {
            int st2;
            waitpid(p2, &st2, 0);
            printf("  \033[1;36m[Tier 2]\033[0m Reaped Grandchild (PID %d) with exit code %d\n", p2, WEXITSTATUS(st2));
            exit(102);
        }
    } else {
        int st1;
        waitpid(p1, &st1, 0);
        printf("\033[1;34m[Tier 1: Grandparent]\033[0m Reaped Child (PID %d) with exit code %d\n", p1, WEXITSTATUS(st1));
        printf("\n\033[1;32m[TREE COMPLETE]\033[0m Complete 3-level tree cleanly resolved without orphans or zombies.\n");
    }

    return 0;
}
