% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly executed under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 2605.687335386508500 ; 2602.682366129790400 ];

%-- Principal point:
cc = [ 1318.256028375271600 ; 963.034468415402670 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ 0.120364528997920 ; -0.387787758809519 ; -0.000075386766850 ; 0.000252004803206 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 11.213366970734604 ; 11.063332136355754 ];

%-- Principal point uncertainty:
cc_error = [ 8.360549028915875 ; 8.282581843807513 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.019344662905215 ; 0.158506677376814 ; 0.001390574843771 ; 0.001431150011990 ; 0.000000000000000 ];

%-- Image size:
nx = 2592;
ny = 1944;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 20;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ 2.202438e+00 ; -2.195393e+00 ; 3.017062e-02 ];
Tc_1  = [ 9.896102e+01 ; 6.734693e+01 ; 3.802930e+02 ];
omc_error_1 = [ 3.436776e-03 ; 3.761569e-03 ; 7.902927e-03 ];
Tc_error_1  = [ 1.230817e+00 ; 1.229728e+00 ; 1.715799e+00 ];

%-- Image #2:
omc_2 = [ -2.110882e+00 ; 2.162780e+00 ; -3.923034e-01 ];
Tc_2  = [ 7.990402e+01 ; 5.740765e+01 ; 3.935855e+02 ];
omc_error_2 = [ 3.349254e-03 ; 2.749660e-03 ; 6.410824e-03 ];
Tc_error_2  = [ 1.263329e+00 ; 1.259957e+00 ; 1.647557e+00 ];

%-- Image #3:
omc_3 = [ 2.033936e+00 ; -1.993965e+00 ; -2.893982e-01 ];
Tc_3  = [ 8.055092e+01 ; 7.781519e+01 ; 3.506435e+02 ];
omc_error_3 = [ 3.151603e-03 ; 2.814641e-03 ; 5.690835e-03 ];
Tc_error_3  = [ 1.148263e+00 ; 1.135037e+00 ; 1.579704e+00 ];

%-- Image #4:
omc_4 = [ 1.970117e+00 ; -1.992795e+00 ; 4.544795e-01 ];
Tc_4  = [ 8.470299e+01 ; 3.822939e+01 ; 3.374491e+02 ];
omc_error_4 = [ 2.392827e-03 ; 3.218347e-03 ; 5.293454e-03 ];
Tc_error_4  = [ 1.083900e+00 ; 1.087517e+00 ; 1.542825e+00 ];

%-- Image #5:
omc_5 = [ -2.114381e+00 ; 2.125434e+00 ; 1.342797e-01 ];
Tc_5  = [ 9.107419e+01 ; 7.482008e+01 ; 3.931080e+02 ];
omc_error_5 = [ 3.243096e-03 ; 3.298390e-03 ; 6.843164e-03 ];
Tc_error_5  = [ 1.266618e+00 ; 1.257270e+00 ; 1.667108e+00 ];

%-- Image #6:
omc_6 = [ 1.837009e+00 ; -1.977417e+00 ; 5.205318e-02 ];
Tc_6  = [ 7.133045e+01 ; 5.524121e+01 ; 3.312403e+02 ];
omc_error_6 = [ 2.695482e-03 ; 2.946269e-03 ; 5.045749e-03 ];
Tc_error_6  = [ 1.076565e+00 ; 1.068695e+00 ; 1.570630e+00 ];

%-- Image #7:
omc_7 = [ -2.112375e+00 ; 1.780008e+00 ; -4.615313e-01 ];
Tc_7  = [ 6.182275e+01 ; 1.022039e+02 ; 4.068004e+02 ];
omc_error_7 = [ 3.085607e-03 ; 2.301443e-03 ; 5.207347e-03 ];
Tc_error_7  = [ 1.316083e+00 ; 1.296090e+00 ; 1.569272e+00 ];

%-- Image #8:
omc_8 = [ 1.778778e+00 ; -2.239183e+00 ; -2.823869e-01 ];
Tc_8  = [ 7.714281e+01 ; 5.694044e+01 ; 3.554464e+02 ];
omc_error_8 = [ 2.943106e-03 ; 3.128769e-03 ; 5.674344e-03 ];
Tc_error_8  = [ 1.156947e+00 ; 1.140177e+00 ; 1.610342e+00 ];

%-- Image #9:
omc_9 = [ 1.960534e+00 ; -1.973263e+00 ; 4.355613e-01 ];
Tc_9  = [ 8.686460e+01 ; 4.120834e+01 ; 3.078203e+02 ];
omc_error_9 = [ 2.332709e-03 ; 3.143118e-03 ; 5.142011e-03 ];
Tc_error_9  = [ 9.901346e-01 ; 9.959759e-01 ; 1.419424e+00 ];

%-- Image #10:
omc_10 = [ 2.186931e+00 ; -2.161130e+00 ; 1.309371e-01 ];
Tc_10  = [ 8.879738e+01 ; 7.521632e+01 ; 3.255549e+02 ];
omc_error_10 = [ 2.903425e-03 ; 3.158593e-03 ; 6.489674e-03 ];
Tc_error_10  = [ 1.058558e+00 ; 1.056837e+00 ; 1.461864e+00 ];

%-- Image #11:
omc_11 = [ 2.188583e+00 ; -2.171063e+00 ; 2.223151e-01 ];
Tc_11  = [ 8.205308e+01 ; 4.351282e+01 ; 4.884920e+02 ];
omc_error_11 = [ 3.919073e-03 ; 4.396782e-03 ; 8.895281e-03 ];
Tc_error_11  = [ 1.568841e+00 ; 1.565335e+00 ; 2.219511e+00 ];

%-- Image #12:
omc_12 = [ 2.023381e+00 ; -2.042067e+00 ; -8.173412e-02 ];
Tc_12  = [ 7.112395e+01 ; 3.985035e+01 ; 4.325780e+02 ];
omc_error_12 = [ 3.371077e-03 ; 3.561470e-03 ; 6.747140e-03 ];
Tc_error_12  = [ 1.393684e+00 ; 1.387068e+00 ; 1.980924e+00 ];

%-- Image #13:
omc_13 = [ -1.984362e+00 ; 2.002545e+00 ; -4.146241e-01 ];
Tc_13  = [ 7.016702e+01 ; 7.552667e+01 ; 4.740586e+02 ];
omc_error_13 = [ 3.237843e-03 ; 2.843795e-03 ; 5.950837e-03 ];
Tc_error_13  = [ 1.523016e+00 ; 1.508839e+00 ; 1.858468e+00 ];

%-- Image #14:
omc_14 = [ 1.902282e+00 ; -1.992016e+00 ; -2.535167e-01 ];
Tc_14  = [ 5.514915e+01 ; 7.375458e+01 ; 3.937236e+02 ];
omc_error_14 = [ 3.192476e-03 ; 2.883733e-03 ; 5.533996e-03 ];
Tc_error_14  = [ 1.278170e+00 ; 1.259626e+00 ; 1.828042e+00 ];

%-- Image #15:
omc_15 = [ 2.124547e+00 ; -2.108991e+00 ; 1.405221e-01 ];
Tc_15  = [ 8.005952e+01 ; 8.582809e+01 ; 3.596840e+02 ];
omc_error_15 = [ 3.269383e-03 ; 3.333962e-03 ; 6.851159e-03 ];
Tc_error_15  = [ 1.175058e+00 ; 1.168284e+00 ; 1.628933e+00 ];

%-- Image #16:
omc_16 = [ -2.088656e+00 ; 2.037731e+00 ; 2.442726e-01 ];
Tc_16  = [ 7.203636e+01 ; 9.211729e+01 ; 4.024128e+02 ];
omc_error_16 = [ 2.802199e-03 ; 3.216687e-03 ; 6.085154e-03 ];
Tc_error_16  = [ 1.301387e+00 ; 1.280649e+00 ; 1.635280e+00 ];

%-- Image #17:
omc_17 = [ -1.987045e+00 ; 2.374241e+00 ; 3.393136e-01 ];
Tc_17  = [ 9.418467e+01 ; 8.276221e+01 ; 3.985257e+02 ];
omc_error_17 = [ 2.687019e-03 ; 3.773337e-03 ; 6.936573e-03 ];
Tc_error_17  = [ 1.293769e+00 ; 1.276578e+00 ; 1.724387e+00 ];

%-- Image #18:
omc_18 = [ -1.894866e+00 ; 2.266459e+00 ; -5.310068e-01 ];
Tc_18  = [ 8.695409e+01 ; 6.230946e+01 ; 4.087592e+02 ];
omc_error_18 = [ 3.023928e-03 ; 2.804607e-03 ; 5.882584e-03 ];
Tc_error_18  = [ 1.314610e+00 ; 1.306925e+00 ; 1.660664e+00 ];

%-- Image #19:
omc_19 = [ 2.148669e+00 ; -2.039804e+00 ; -4.460041e-01 ];
Tc_19  = [ 7.030910e+01 ; 7.711183e+01 ; 4.243145e+02 ];
omc_error_19 = [ 3.603065e-03 ; 2.947684e-03 ; 6.538531e-03 ];
Tc_error_19  = [ 1.374266e+00 ; 1.358062e+00 ; 1.849983e+00 ];

%-- Image #20:
omc_20 = [ -2.197657e+00 ; 2.214525e+00 ; 2.553238e-02 ];
Tc_20  = [ 7.967563e+01 ; 7.729490e+01 ; 4.320071e+02 ];
omc_error_20 = [ 4.213464e-03 ; 4.280750e-03 ; 9.255349e-03 ];
Tc_error_20  = [ 1.396365e+00 ; 1.383643e+00 ; 1.939253e+00 ];

