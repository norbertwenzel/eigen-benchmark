#include <nonius/nonius.h++>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <cassert>
#include <limits>
#include <random>

#ifdef NDEBUG
static constexpr std::size_t points_per_matrix = 50;
#else
static constexpr std::size_t points_per_matrix = 500000;
#endif
static constexpr unsigned int seed = 0xdeadbeef;
static constexpr unsigned int dims = 3;

template<typename Scalar>
using pointlist = Eigen::Matrix<Scalar, dims, Eigen::Dynamic, Eigen::RowMajor>;
template<typename Scalar>
using point = Eigen::Matrix<Scalar, dims, 1>;
template<typename Scalar>
using transform = Eigen::Transform<Scalar, dims, Eigen::Isometry>;

template<typename Scalar>
pointlist<Scalar> create_random_points(const std::size_t num_points)
{
    std::default_random_engine eng{seed};
    std::uniform_real_distribution<Scalar> dist{0, 1};

    assert(num_points <= std::numeric_limits<typename pointlist<Scalar>::Index>::max());
    pointlist<Scalar> pts{dims, static_cast<typename pointlist<Scalar>::Index>(num_points)};

    for(auto c = pts.cols(); c > 0; --c)
    {
        pts.col(c-1) = point<Scalar>{dist(eng), dist(eng), dist(eng)};
    }

    return pts;
}

template<typename Scalar>
transform<Scalar> create_random_transformation()
{
    std::default_random_engine eng{seed};
    std::normal_distribution<Scalar> ndist{};
    std::uniform_real_distribution<Scalar> dist{-10, 10};

    using quaternion = Eigen::Quaternion<Scalar>;
    using translation = Eigen::Translation<Scalar, dims>;

    transform<Scalar> rand_trans;
    rand_trans = translation{typename translation::VectorType{dist(eng), dist(eng), dist(eng)}};
    rand_trans.rotate(quaternion{ndist(eng), ndist(eng), ndist(eng), ndist(eng)}.normalized());
    return rand_trans;
}

template<typename Scalar>
pointlist<Scalar> eigen_matrix_transformation(pointlist<Scalar> pts,
                                                     const transform<Scalar> &trans)
{
    return (trans * pts.colwise().homogeneous());
}

template<typename Scalar>
pointlist<Scalar> raw_loop_transformation(pointlist<Scalar> pts,
                                                 const transform<Scalar> &trans)
{
#ifndef NDEBUG
    pointlist<Scalar> check = eigen_matrix_transformation(pts, trans);
#endif

    for(auto c = pts.cols(); c > 0; --c)
    {
        pts.col(c-1) = trans * pts.col(c-1).homogeneous();
    }

#ifndef NDEBUG
    assert(pts.isApprox(check));
#endif

    return pts;
}


NONIUS_BENCHMARK("raw loop double",
                 [](nonius::chronometer meter)
                 {
                     using scalar = double;
                     const pointlist<scalar> pts = create_random_points<scalar>(points_per_matrix);
                     const transform<scalar> t = create_random_transformation<scalar>();

                     meter.measure([&]{ raw_loop_transformation<scalar>(pts, t); });
                 })

NONIUS_BENCHMARK("eigen matrix double",
                 [](nonius::chronometer meter)
                 {
                     using scalar = double;
                     const pointlist<scalar> pts = create_random_points<scalar>(points_per_matrix);
                     const transform<scalar> t = create_random_transformation<scalar>();

                     meter.measure([&]{ eigen_matrix_transformation<scalar>(pts, t); });
                 })

NONIUS_BENCHMARK("raw loop float",
                 [](nonius::chronometer meter)
                 {
                     using scalar = float;
                     const pointlist<scalar> pts = create_random_points<scalar>(points_per_matrix);
                     const transform<scalar> t = create_random_transformation<scalar>();

                     meter.measure([&]{ raw_loop_transformation<scalar>(pts, t); });
                 })

NONIUS_BENCHMARK("eigen matrix float",
                 [](nonius::chronometer meter)
                 {
                     using scalar = float;
                     const pointlist<scalar> pts = create_random_points<scalar>(points_per_matrix);
                     const transform<scalar> t = create_random_transformation<scalar>();

                     meter.measure([&]{ eigen_matrix_transformation<scalar>(pts, t); });
                 })
