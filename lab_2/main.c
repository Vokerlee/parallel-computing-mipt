#include <math.h>
#include <string.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sysexits.h>
#include <err.h>
#include <float.h>
#include <stdint.h>

#define X_START 0.0
#define X_END   1.0

#define Y_START 0.0
#define Y_MAX   1.0

#define BOUNDARY_STEP 0.1
#define N_BOUNDARIES (size_t) (((Y_MAX) - (Y_START)) / (BOUNDARY_STEP) + 1)

#define N_ITERS 5000

#define N ((1 << 12) - 1)
#define H_STEP ((X_END) - (X_START)) / (N)

double get_residue_norm_L1(const double vec1[], const double vec2[], size_t vec_dim)
{
    double distance = 0;
    for (size_t i = 0; i < vec_dim; ++i)
        distance += fabs(vec1[i] - vec2[i]);

    return distance;
}

// ! www.youtube.com/watch?v=Qk_4MnAOgU4&list=PLthfp5exSWEqzHAKU1OsSfTNlNWULhos0&index=11?t=15m50s
void reduction_algorithm(double a[N], double b[N], double c[N], double f[N], double y[N]) {

    #pragma omp parallel
    {
        size_t stride = 1;
        size_t start_idx = 2;

        for (size_t rec_step = N; rec_step > 1; rec_step /= 2, start_idx *= 2, stride *= 2)
        {
            #pragma omp for
            for (size_t i = start_idx - 1; i < N; i += stride * 2)
            {
                const double k1 = -a[i] / b[i - stride];
                const double k2 = -c[i] / b[i + stride];

                a[i] = k1 * a[i - stride];
                b[i] = k1 * c[i - stride] + b[i] + k2 * a[i + stride];
                c[i] = k2 * c[i + stride];
                f[i] = k1 * f[i - stride] + f[i] + k2 * f[i + stride];
            }
        }

        #pragma omp barrier

        y[N / 2] = f[N / 2] / b[N / 2];

        for (stride /= 2; stride >= 1; stride /= 2)
        {
            #pragma omp for
            for (size_t i = stride - 1; i < N; i += stride * 2)
            {
                y[i] = (f[i] - (((i - stride) > 0) ? a[i] * y[i - stride] : 0.0)
                             - (((i + stride) < N) ? c[i] * y[i + stride] : 0.0)) / b[i];
            }
        }
    }
}

int main(int argc, char *argv[])
{
    size_t n_threads = 1;
    double epsilon = 1e-16;

    errno = 0;

    if (argc >= 2)
    {
        n_threads = strtol(argv[1], NULL, 10);
        if (errno == EINVAL)
            errx(EX_USAGE, "Invalid number of threads: %s", strerror(errno));

        if (argc >= 3)
        {
            epsilon = strtod(argv[1], NULL);
            if (errno == EINVAL)
                errx(EX_USAGE, "Invalid accuracy parameter (epsilon): %s", strerror(errno));
        }
    }

    omp_set_num_threads(n_threads);

    double x[N]      = {0};
    double y_prev[N] = {0};
    double y_next[N] = {0};
    double a[N]      = {0};
    double b[N]      = {0};
    double c[N]      = {0};
    double f[N]      = {0};

    double solution_of_b[N_BOUNDARIES][N];

    for (size_t sol = 0; sol < N_BOUNDARIES; ++sol)
    {
        double right_y_bound = sol * BOUNDARY_STEP;

        for (size_t i = 0; i < N; ++i)
        {
            double coeff = 1;
            x[i] = X_START + i * H_STEP;
            y_prev[i] = 1 + (right_y_bound - 1 - coeff) * x[i] + coeff * x[i] * x[i];
        }

        const double start_time = omp_get_wtime();

        // Main iterative loop
        for (size_t iter = 0; iter < N_ITERS; ++iter)
        {
            const double h2_per_12 = H_STEP * H_STEP / 12.0;

            a[0] = 0.0;
            b[0] = 1.0;
            c[0] = 0.0;
            f[0] = 1.0;

            #pragma omp parallel for schedule(static, 3)
            for (size_t n = 1; n < N - 1; ++n)
            {
                double exp_y_n_prev = exp(y_prev[n - 1]);
                double exp_y_n_cur  = exp(y_prev[n]);
                double exp_y_n_next = exp(y_prev[n + 1]);

                a[n] =  1.0 - 1.0  * h2_per_12 * exp_y_n_next;
                b[n] = -2.0 - 10.0 * h2_per_12 * exp_y_n_cur;
                c[n] =  1.0 - 1.0  * h2_per_12 * exp_y_n_prev;
                f[n] = h2_per_12 * (exp_y_n_next * (1.0 - y_prev[n + 1]) + 10.0 * exp_y_n_cur * (1.0 - y_prev[n]) + exp_y_n_prev * (1.0 - y_prev[n - 1]));
            }

            a[N - 1] = 0.0;
            b[N - 1] = 1.0;
            c[N - 1] = 0.0;
            f[N - 1] = right_y_bound;

            reduction_algorithm(a, b, c, f, y_next);

            if (get_residue_norm_L1(y_prev, y_next, N) < epsilon)
                break;

            memcpy(y_prev, y_next, sizeof(y_next));
        }

        const double end_time = omp_get_wtime();

        memcpy(solution_of_b[sol], y_prev, sizeof(y_prev));

        // printf("X range: [0, 1]\n");
        // printf("Y range: [0, %lf]\n", right_y_bound);
        // printf("Epsilon: %.*e\n", DECIMAL_DIG, epsilon);
        // printf("Time: %lf sec\n", end_time - start_time);
    }

    printf("x");
    for (size_t sol = 0; sol < N_BOUNDARIES; ++sol)
        printf(",y%ld", sol);

    printf("\n");

    for (size_t i = 0; i < N; ++i)
    {
        printf("%lf", x[i]);
        for (size_t sol = 0; sol < N_BOUNDARIES; ++sol)
            printf(",%lf", solution_of_b[sol][i]);

        printf("\n");
    }

    return EXIT_SUCCESS;
}
