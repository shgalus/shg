/* strdlr.cc: NIST StRD Linear Regression */

/**
 * \file src/strdlr.cc
 * NIST StRD Linear Regression.
 * \date Created on 12 August 2014.
 */

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include "shg/strdlr.h"

namespace SHG {

void StRDLR::active(int i) {
     if (i < 0 || static_cast<std::size_t>(i) >= v_.size())
          throw std::invalid_argument(__func__);
     d_ = &v_[i];
}

Matdouble StRDLR::X() {
     const int i = d_ - &v_[0];
     if (i == norris || i == noint1 || i == noint2 || i == longley)
          return d_->X;
     Matdouble X(d_->n, d_->k);
     for (std::size_t i = 0; i < X.nrows(); i++)
          X(i, 0) = d_->X(i, 0);
     for (std::size_t i = 0; i < X.nrows(); i++)
          for (std::size_t j = 1; j < X.ncols(); j++)
               X(i, j) = X(i, j - 1) * X(i, 0);
     return X;
}

const std::vector<StRDLR::Data> StRDLR::v_ = {
     {
          "Norris",             // i = 0
          36,                   // number of observations n
          1,                    // number of columns in X k
          {36, 1,
           {                    // predictor variables X
                0.2,
                337.4,
                118.2,
                884.6,
                10.1,
                226.5,
                666.3,
                996.3,
                448.6,
                777.0,
                558.2,
                0.4,
                0.6,
                775.5,
                666.9,
                338.0,
                447.5,
                11.6,
                556.0,
                228.1,
                995.8,
                887.6,
                120.2,
                0.3,
                0.3,
                556.8,
                339.1,
                887.2,
                999.0,
                779.0,
                11.1,
                118.3,
                229.2,
                669.1,
                448.9,
                0.5,
           }},
          {                     // response variable y
               0.1,
               338.8,
               118.1,
               888.0,
               9.2,
               228.1,
               668.5,
               998.5,
               449.1,
               778.9,
               559.2,
               0.3,
               0.1,
               778.1,
               668.8,
               339.3,
               448.9,
               10.8,
               557.7,
               228.3,
               998.0,
               888.8,
               119.6,
               0.3,
               0.6,
               557.6,
               339.3,
               888.0,
               998.5,
               778.9,
               10.2,
               117.6,
               228.9,
               668.4,
               449.2,
               0.2,
          },
          {                     // estimates b
               -0.262323073774029,
               1.00211681802045,
          },
          {                     // standard deviations of estimates
               0.232818234301152,
               0.429796848199937E-03,
          },
          0.884796396144373,    // residual standard deviation sdevres
          0.999993745883712,    // R-squared r2
          1,                    // regression degrees of freedom dfreg
          4255954.13232369,     // regression sum of squares ssreg
          4255954.13232369,     // regression mean squares msreg
          5436385.54079785,     // F statistic fstat
          34,                   // residual degrees of freedom dfres
          26.6173985294224,     // residual sum of squares ssres
          0.782864662630069,    // residual mean squares msres
     },
     {
          "Pontius",            // i = 1
          40,                   // number of observations n
          2,                    // number of columns in X k
          {40, 1,
           {                    // predictor variables X
                150000,
                300000,
                450000,
                600000,
                750000,
                900000,
                1050000,
                1200000,
                1350000,
                1500000,
                1650000,
                1800000,
                1950000,
                2100000,
                2250000,
                2400000,
                2550000,
                2700000,
                2850000,
                3000000,
                150000,
                300000,
                450000,
                600000,
                750000,
                900000,
                1050000,
                1200000,
                1350000,
                1500000,
                1650000,
                1800000,
                1950000,
                2100000,
                2250000,
                2400000,
                2550000,
                2700000,
                2850000,
                3000000,
           }},
          {                     // response variable y
               .11019,
               .21956,
               .32949,
               .43899,
               .54803,
               .65694,
               .76562,
               .87487,
               .98292,
               1.09146,
               1.20001,
               1.30822,
               1.41599,
               1.52399,
               1.63194,
               1.73947,
               1.84646,
               1.95392,
               2.06128,
               2.16844,
               .11052,
               .22018,
               .32939,
               .43886,
               .54798,
               .65739,
               .76596,
               .87474,
               .98300,
               1.09150,
               1.20004,
               1.30818,
               1.41613,
               1.52408,
               1.63159,
               1.73965,
               1.84696,
               1.95445,
               2.06177,
               2.16829,
          },
          {                     // estimates b
               0.673565789473684E-03,
               0.732059160401003E-06,
               -0.316081871345029E-14,
          },
          {                     // standard deviations of estimates
               0.107938612033077E-03,
               0.157817399981659E-09,
               0.486652849992036E-16,
          },
          0.205177424076185E-03,  // residual standard deviation sdevres
          0.999999900178537,    // R-squared r2
          2,                    // regression degrees of freedom dfreg
          15.6040343244198,     // regression sum of squares ssreg
          7.80201716220991,     // regression mean squares msreg
          185330865.995752,     // F statistic fstat
          37,                   // residual degrees of freedom dfres
          0.155761768796992E-05,  // residual sum of squares ssres
          0.420977753505385E-07,  // residual mean squares msres
     },
     {
          "NoInt1",             // i = 2
          11,                   // number of observations n
          1,                    // number of columns in X k
          {11, 1,
           {                    // predictor variables X
                60,
                61,
                62,
                63,
                64,
                65,
                66,
                67,
                68,
                69,
                70,
           }},
          {                     // response variable y
               130,
               131,
               132,
               133,
               134,
               135,
               136,
               137,
               138,
               139,
               140,
          },
          {                     // estimates b
               2.07438016528926,
          },
          {                     // standard deviations of estimates
               0.165289256198347E-01,
          },
          3.56753034006338,     // residual standard deviation sdevres
          0.999365492298663,    // R-squared r2
          1,                    // regression degrees of freedom dfreg
          200457.727272727,     // regression sum of squares ssreg
          200457.727272727,     // regression mean squares msreg
          15750.2500000000,     // F statistic fstat
          10,                   // residual degrees of freedom dfres
          127.272727272727,     // residual sum of squares ssres
          12.7272727272727,     // residual mean squares msres
     },
     {
          "NoInt2",             // i = 3
          3,                    // number of observations n
          1,                    // number of columns in X k
          {3, 1,
           {                    // predictor variables X
                4,
                5,
                6,
           }},
          {                     // response variable y
               3,
               4,
               4,
          },
          {                     // estimates b
               0.727272727272727,
          },
          {                     // standard deviations of estimates
               0.420827318078432E-01,
          },
          0.369274472937998,    // residual standard deviation sdevres
          0.993348115299335,    // R-squared r2
          1,                    // regression degrees of freedom dfreg
          40.7272727272727,     // regression sum of squares ssreg
          40.7272727272727,     // regression mean squares msreg
          298.6666666666667,    // F statistic fstat
          2,                    // residual degrees of freedom dfres
          0.272727272727273,    // residual sum of squares ssres
          0.136363636363636,    // residual mean squares msres
     },
     {
          "Filip",              // i = 4
          82,                   // number of observations n
          10,                   // number of columns in X k
          {82, 1,
           {                    // predictor variables X
                -6.860120914,
                -4.324130045,
                -4.358625055,
                -4.358426747,
                -6.955852379,
                -6.661145254,
                -6.355462942,
                -6.118102026,
                -7.115148017,
                -6.815308569,
                -6.519993057,
                -6.204119983,
                -5.853871964,
                -6.109523091,
                -5.79832982,
                -5.482672118,
                -5.171791386,
                -4.851705903,
                -4.517126416,
                -4.143573228,
                -3.709075441,
                -3.499489089,
                -6.300769497,
                -5.953504836,
                -5.642065153,
                -5.031376979,
                -4.680685696,
                -4.329846955,
                -3.928486195,
                -8.56735134,
                -8.363211311,
                -8.107682739,
                -7.823908741,
                -7.522878745,
                -7.218819279,
                -6.920818754,
                -6.628932138,
                -6.323946875,
                -5.991399828,
                -8.781464495,
                -8.663140179,
                -8.473531488,
                -8.247337057,
                -7.971428747,
                -7.676129393,
                -7.352812702,
                -7.072065318,
                -6.774174009,
                -6.478861916,
                -6.159517513,
                -6.835647144,
                -6.53165267,
                -6.224098421,
                -5.910094889,
                -5.598599459,
                -5.290645224,
                -4.974284616,
                -4.64454848,
                -4.290560426,
                -3.885055584,
                -3.408378962,
                -3.13200249,
                -8.726767166,
                -8.66695597,
                -8.511026475,
                -8.165388579,
                -7.886056648,
                -7.588043762,
                -7.283412422,
                -6.995678626,
                -6.691862621,
                -6.392544977,
                -6.067374056,
                -6.684029655,
                -6.378719832,
                -6.065855188,
                -5.752272167,
                -5.132414673,
                -4.811352704,
                -4.098269308,
                -3.66174277,
                -3.2644011,
           }},
          {                     // response variable y
               0.8116,
               0.9072,
               0.9052,
               0.9039,
               0.8053,
               0.8377,
               0.8667,
               0.8809,
               0.7975,
               0.8162,
               0.8515,
               0.8766,
               0.8885,
               0.8859,
               0.8959,
               0.8913,
               0.8959,
               0.8971,
               0.9021,
               0.909,
               0.9139,
               0.9199,
               0.8692,
               0.8872,
               0.89,
               0.891,
               0.8977,
               0.9035,
               0.9078,
               0.7675,
               0.7705,
               0.7713,
               0.7736,
               0.7775,
               0.7841,
               0.7971,
               0.8329,
               0.8641,
               0.8804,
               0.7668,
               0.7633,
               0.7678,
               0.7697,
               0.77,
               0.7749,
               0.7796,
               0.7897,
               0.8131,
               0.8498,
               0.8741,
               0.8061,
               0.846,
               0.8751,
               0.8856,
               0.8919,
               0.8934,
               0.894,
               0.8957,
               0.9047,
               0.9129,
               0.9209,
               0.9219,
               0.7739,
               0.7681,
               0.7665,
               0.7703,
               0.7702,
               0.7761,
               0.7809,
               0.7961,
               0.8253,
               0.8602,
               0.8809,
               0.8301,
               0.8664,
               0.8834,
               0.8898,
               0.8964,
               0.8963,
               0.9074,
               0.9119,
               0.9228,
          },
          {                     // estimates b
               -1467.48961422980,
               -2772.17959193342,
               -2316.37108160893,
               -1127.97394098372,
               -354.478233703349,
               -75.1242017393757,
               -10.8753180355343,
               -1.06221498588947,
               -0.670191154593408E-01,
               -0.246781078275479E-02,
               -0.402962525080404E-04,
          },
          {                     // standard deviations of estimates
               298.084530995537,
               559.779865474950,
               466.477572127796,
               227.204274477751,
               71.6478660875927,
               15.2897178747400,
               2.23691159816033,
               0.221624321934227,
               0.142363763154724E-01,
               0.535617408889821E-03,
               0.896632837373868E-05,
          },
          0.334801051324544E-02,  // residual standard deviation sdevres
          0.996727416185620,    // R-squared r2
          10,                   // regression degrees of freedom dfreg
          0.242391619837339,    // regression sum of squares ssreg
          0.242391619837339E-01,  // regression mean squares msreg
          2162.43954511489,     // F statistic fstat
          71,                   // residual degrees of freedom dfres
          0.795851382172941E-03,  // residual sum of squares ssres
          0.112091743968020E-04,  // residual mean squares msres
     },
     {
          "Longley",            // i = 5
          16,                   // number of observations n
          6,                    // number of columns in X k
          {16, 6,
           {                    // predictor variables X
                83.0,
                234289,
                2356,
                1590,
                107608,
                1947,
                88.5,
                259426,
                2325,
                1456,
                108632,
                1948,
                88.2,
                258054,
                3682,
                1616,
                109773,
                1949,
                89.5,
                284599,
                3351,
                1650,
                110929,
                1950,
                96.2,
                328975,
                2099,
                3099,
                112075,
                1951,
                98.1,
                346999,
                1932,
                3594,
                113270,
                1952,
                99.0,
                365385,
                1870,
                3547,
                115094,
                1953,
                100.0,
                363112,
                3578,
                3350,
                116219,
                1954,
                101.2,
                397469,
                2904,
                3048,
                117388,
                1955,
                104.6,
                419180,
                2822,
                2857,
                118734,
                1956,
                108.4,
                442769,
                2936,
                2798,
                120445,
                1957,
                110.8,
                444546,
                4681,
                2637,
                121950,
                1958,
                112.6,
                482704,
                3813,
                2552,
                123366,
                1959,
                114.2,
                502601,
                3931,
                2514,
                125368,
                1960,
                115.7,
                518173,
                4806,
                2572,
                127852,
                1961,
                116.9,
                554894,
                4007,
                2827,
                130081,
                1962,
           }},
          {                     // response variable y
               60323,
               61122,
               60171,
               61187,
               63221,
               63639,
               64989,
               63761,
               66019,
               67857,
               68169,
               66513,
               68655,
               69564,
               69331,
               70551,
          },
          {                     // estimates b
               -3482258.63459582,
               15.0618722713733,
               -0.358191792925910E-01,
               -2.02022980381683,
               -1.03322686717359,
               -0.511041056535807E-01,
               1829.15146461355,
          },
          {                     // standard deviations of estimates
               890420.383607373,
               84.9149257747669,
               0.334910077722432E-01,
               0.488399681651699,
               0.214274163161675,
               0.226073200069370,
               455.478499142212,
          },
          304.854073561965,     // residual standard deviation sdevres
          0.995479004577296,    // R-squared r2
          6,                    // regression degrees of freedom dfreg
          184172401.944494,     // regression sum of squares ssreg
          30695400.3240823,     // regression mean squares msreg
          330.285339234588,     // F statistic fstat
          9,                    // residual degrees of freedom dfres
          836424.055505915,     // residual sum of squares ssres
          92936.0061673238,     // residual mean squares msres
     },
     {
          "Wampler1",           // i = 6
          21,                   // number of observations n
          5,                    // number of columns in X k
          {21, 1,
           {                    // predictor variables X
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                10,
                11,
                12,
                13,
                14,
                15,
                16,
                17,
                18,
                19,
                20,
           }},
          {                     // response variable y
               1,
               6,
               63,
               364,
               1365,
               3906,
               9331,
               19608,
               37449,
               66430,
               111111,
               177156,
               271453,
               402234,
               579195,
               813616,
               1118481,
               1508598,
               2000719,
               2613660,
               3368421,
          },
          {                     // estimates b
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
          },
          {                     // standard deviations of estimates
               0.000000000000000,
               0.000000000000000,
               0.000000000000000,
               0.000000000000000,
               0.000000000000000,
               0.000000000000000,
          },
          0.000000000000000,    // residual standard deviation sdevres
          1.00000000000000,     // R-squared r2
          5,                    // regression degrees of freedom dfreg
          18814317208116.7,     // regression sum of squares ssreg
          3762863441623.33,     // regression mean squares msreg
          std::numeric_limits<double>::infinity(),  // F statistic fstat
          15,                   // residual degrees of freedom dfres
          0.000000000000000,    // residual sum of squares ssres
          0.000000000000000,    // residual mean squares msres
     },
     {
          "Wampler2",           // i = 7
          21,                   // number of observations n
          5,                    // number of columns in X k
          {21, 1,
           {                    // predictor variables X
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                10,
                11,
                12,
                13,
                14,
                15,
                16,
                17,
                18,
                19,
                20,
           }},
          {                     // response variable y
               1.00000,
               1.11111,
               1.24992,
               1.42753,
               1.65984,
               1.96875,
               2.38336,
               2.94117,
               3.68928,
               4.68559,
               6.00000,
               7.71561,
               9.92992,
               12.75603,
               16.32384,
               20.78125,
               26.29536,
               33.05367,
               41.26528,
               51.16209,
               63.00000,
          },
          {                     // estimates b
               1.00000000000000,
               0.100000000000000,
               0.100000000000000E-01,
               0.100000000000000E-02,
               0.100000000000000E-03,
               0.100000000000000E-04,
          },
          {                     // standard deviations of estimates
               0.000000000000000,
               0.000000000000000,
               0.000000000000000,
               0.000000000000000,
               0.000000000000000,
               0.000000000000000,
          },
          0.000000000000000,    // residual standard deviation sdevres
          1.00000000000000,     // R-squared r2
          5,                    // regression degrees of freedom dfreg
          6602.91858365167,     // regression sum of squares ssreg
          1320.58371673033,     // regression mean squares msreg
          std::numeric_limits<double>::infinity(),  // F statistic fstat
          15,                   // residual degrees of freedom dfres
          0.000000000000000,    // residual sum of squares ssres
          0.000000000000000,    // residual mean squares msres
     },
     {
          "Wampler3",           // i = 8
          21,                   // number of observations n
          5,                    // number of columns in X k
          {21, 1,
           {                    // predictor variables X
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                10,
                11,
                12,
                13,
                14,
                15,
                16,
                17,
                18,
                19,
                20,
           }},
          {                     // response variable y
               760.,
               -2042.,
               2111.,
               -1684.,
               3888.,
               1858.,
               11379.,
               17560.,
               39287.,
               64382.,
               113159.,
               175108.,
               273291.,
               400186.,
               581243.,
               811568.,
               1121004.,
               1506550.,
               2002767.,
               2611612.,
               3369180.,
          },
          {                     // estimates b
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
          },
          {                     // standard deviations of estimates
               2152.32624678170,
               2363.55173469681,
               779.343524331583,
               101.475507550350,
               5.64566512170752,
               0.112324854679312,
          },
          2360.14502379268,     // residual standard deviation sdevres
          0.999995559025820,    // R-squared r2
          5,                    // regression degrees of freedom dfreg
          18814317208116.7,     // regression sum of squares ssreg
          3762863441623.33,     // regression mean squares msreg
          675524.458240122,     // F statistic fstat
          15,                   // residual degrees of freedom dfres
          83554268.0000000,     // residual sum of squares ssres
          5570284.53333333,     // residual mean squares msres
     },
     {
          "Wampler4",           // i = 9
          21,                   // number of observations n
          5,                    // number of columns in X k
          {21, 1,
           {                    // predictor variables X
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                10,
                11,
                12,
                13,
                14,
                15,
                16,
                17,
                18,
                19,
                20,
           }},
          {                     // response variable y
               75901,
               -204794,
               204863,
               -204436,
               253665,
               -200894,
               214131,
               -185192,
               221249,
               -138370,
               315911,
               -27644,
               455253,
               197434,
               783995,
               608816,
               1370781,
               1303798,
               2205519,
               2408860,
               3444321,
          },
          {                     // estimates b
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
          },
          {                     // standard deviations of estimates
               215232.624678170,
               236355.173469681,
               77934.3524331583,
               10147.5507550350,
               564.566512170752,
               11.2324854679312,
          },
          236014.502379268,     // residual standard deviation sdevres
          0.957478440825662,    // R-squared r2
          5,                    // regression degrees of freedom dfreg
          18814317208116.7,     // regression sum of squares ssreg
          3762863441623.33,     // regression mean squares msreg
          67.5524458240122,     // F statistic fstat
          15,                   // residual degrees of freedom dfres
          835542680000.000,     // residual sum of squares ssres
          55702845333.3333,     // residual mean squares msres
     },
     {
          "Wampler5",           // i = 10
          21,                   // number of observations n
          5,                    // number of columns in X k
          {21, 1,
           {                    // predictor variables X
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                10,
                11,
                12,
                13,
                14,
                15,
                16,
                17,
                18,
                19,
                20,
           }},
          {                     // response variable y
               7590001,
               -20479994,
               20480063,
               -20479636,
               25231365,
               -20476094,
               20489331,
               -20460392,
               18417449,
               -20413570,
               20591111,
               -20302844,
               18651453,
               -20077766,
               21059195,
               -19666384,
               26348481,
               -18971402,
               22480719,
               -17866340,
               10958421,
          },
          {                     // estimates b
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
               1.00000000000000,
          },
          {                     // standard deviations of estimates
               21523262.4678170,
               23635517.3469681,
               7793435.24331583,
               1014755.07550350,
               56456.6512170752,
               1123.24854679312,
          },
          23601450.2379268,     // residual standard deviation sdevres
          0.224668921574940E-02,  // R-squared r2
          5,                    // regression degrees of freedom dfreg
          18814317208116.7,     // regression sum of squares ssreg
          3762863441623.33,     // regression mean squares msreg
          6.7552445824012241E-03,  // F statistic fstat
          15,                   // residual degrees of freedom dfres
          0.835542680000000E+16,  // residual sum of squares ssres
          557028453333333.,     // residual mean squares msres
     },
};

}       // namespace SHG
