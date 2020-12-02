#include "plugin.hpp"


struct Capacitor2 : Module {

	double iirHighpassAL = 0.0;
    double iirHighpassBL = 0.0;
    double iirHighpassCL = 0.0;
    double iirHighpassDL = 0.0;
    double iirHighpassEL = 0.0;
    double iirHighpassFL = 0.0;
    double iirLowpassAL = 0.0;
    double iirLowpassBL = 0.0;
    double iirLowpassCL = 0.0;
    double iirLowpassDL = 0.0;
    double iirLowpassEL = 0.0;
    double iirLowpassFL = 0.0;

    double iirHighpassAR = 0.0;
    double iirHighpassBR = 0.0;
    double iirHighpassCR = 0.0;
    double iirHighpassDR = 0.0;
    double iirHighpassER = 0.0;
    double iirHighpassFR = 0.0;
    double iirLowpassAR = 0.0;
    double iirLowpassBR = 0.0;
    double iirLowpassCR = 0.0;
    double iirLowpassDR = 0.0;
    double iirLowpassER = 0.0;
    double iirLowpassFR = 0.0;

    int count = 0;

    double lowpassChase = 0.0;
    double highpassChase = 0.0;
    double wetChase = 0.0;

    double lowpassBaseAmount = 1.0;
    double highpassBaseAmount = 0.0;
    double wet = 1;

    double lastLowpass = 1000.0;
    double lastHighpass = 1000.0;
    double lastWet = 1000.0;

    uint32_t fpd = 17;

	enum ParamIds {
		LOWPASS_PARAM,
		HIGHPASS_PARAM,
		NONLIN_PARAM,
		DRYWET_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1_INPUT,
		INPUT_2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_2_OUTPUT,
		OUTPUT_1_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Capacitor2() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(LOWPASS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(HIGHPASS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(NONLIN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DRYWET_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
		long double inputSampleL = inputs[INPUT_1_INPUT].getVoltage();
        long double inputSampleR = inputs[INPUT_2_INPUT].getVoltage();

        lowpassChase = pow(params[LOWPASS_PARAM].getValue(), 2);
        highpassChase = pow(params[HIGHPASS_PARAM].getValue(), 2);
        double nonLin = 1.0+((1.0-params[NONLIN_PARAM].getValue())*6.0);
        double nonLinTrim = 1.5/cbrt(nonLin);
        wetChase = params[DRYWET_PARAM].getValue();
        //should not scale with sample rate, because values reaching 1 are important
        //to its ability to bypass when set to max
        double lowpassSpeed = 300 / (fabs( lastLowpass - lowpassChase)+1.0);
        double highpassSpeed = 300 / (fabs( lastHighpass - highpassChase)+1.0);
        double wetSpeed = 300 / (fabs( lastWet - wetChase)+1.0);
        lastLowpass = lowpassChase;
        lastHighpass = highpassChase;
        lastWet = wetChase;

        if (fabs(inputSampleL)<1.18e-37) inputSampleL = fpd * 1.18e-37;
        if (fabs(inputSampleR)<1.18e-37) inputSampleR = fpd * 1.18e-37;
        long double drySampleL = inputSampleL;
        long double drySampleR = inputSampleR;

        double dielectricScaleL = fabs(2.0-((inputSampleL+nonLin)/nonLin));
        double dielectricScaleR = fabs(2.0-((inputSampleR+nonLin)/nonLin));

        lowpassBaseAmount = (((lowpassBaseAmount*lowpassSpeed)+lowpassChase)/(lowpassSpeed + 1.0));
        //positive voltage will mean lower capacitance when capacitor is barium titanate
        //on the lowpass, higher pressure means positive swings/smaller cap/larger value for lowpassAmount
        double lowpassAmountL = lowpassBaseAmount * dielectricScaleL;
        double invLowpassL = 1.0 - lowpassAmountL;
        double lowpassAmountR = lowpassBaseAmount * dielectricScaleR;
        double invLowpassR = 1.0 - lowpassAmountR;

        highpassBaseAmount = (((highpassBaseAmount*highpassSpeed)+highpassChase)/(highpassSpeed + 1.0));
        //positive voltage will mean lower capacitance when capacitor is barium titanate
        //on the highpass, higher pressure means positive swings/smaller cap/larger value for highpassAmount
        double highpassAmountL = highpassBaseAmount * dielectricScaleL;
        double invHighpassL = 1.0 - highpassAmountL;
        double highpassAmountR = highpassBaseAmount * dielectricScaleR;
        double invHighpassR = 1.0 - highpassAmountR;

        wet = (((wet*wetSpeed)+wetChase)/(wetSpeed+1.0));

        count++; if (count > 5) count = 0; switch (count)
        {
            case 0:
                iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassAL;
                iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassAL;
                iirHighpassBL = (iirHighpassBL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassBL;
                iirLowpassBL = (iirLowpassBL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassBL;
                iirHighpassDL = (iirHighpassDL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassDL;
                iirLowpassDL = (iirLowpassDL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassDL;
                iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassAR;
                iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassAR;
                iirHighpassBR = (iirHighpassBR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassBR;
                iirLowpassBR = (iirLowpassBR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassBR;
                iirHighpassDR = (iirHighpassDR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassDR;
                iirLowpassDR = (iirLowpassDR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassDR;
                break;
            case 1:
                iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassAL;
                iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassAL;
                iirHighpassCL = (iirHighpassCL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassCL;
                iirLowpassCL = (iirLowpassCL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassCL;
                iirHighpassEL = (iirHighpassEL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassEL;
                iirLowpassEL = (iirLowpassEL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassEL;
                iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassAR;
                iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassAR;
                iirHighpassCR = (iirHighpassCR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassCR;
                iirLowpassCR = (iirLowpassCR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassCR;
                iirHighpassER = (iirHighpassER * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassER;
                iirLowpassER = (iirLowpassER * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassER;
                break;
            case 2:
                iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassAL;
                iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassAL;
                iirHighpassBL = (iirHighpassBL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassBL;
                iirLowpassBL = (iirLowpassBL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassBL;
                iirHighpassFL = (iirHighpassFL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassFL;
                iirLowpassFL = (iirLowpassFL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassFL;
                iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassAR;
                iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassAR;
                iirHighpassBR = (iirHighpassBR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassBR;
                iirLowpassBR = (iirLowpassBR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassBR;
                iirHighpassFR = (iirHighpassFR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassFR;
                iirLowpassFR = (iirLowpassFR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassFR;
                break;
            case 3:
                iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassAL;
                iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassAL;
                iirHighpassCL = (iirHighpassCL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassCL;
                iirLowpassCL = (iirLowpassCL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassCL;
                iirHighpassDL = (iirHighpassDL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassDL;
                iirLowpassDL = (iirLowpassDL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassDL;
                iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassAR;
                iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassAR;
                iirHighpassCR = (iirHighpassCR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassCR;
                iirLowpassCR = (iirLowpassCR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassCR;
                iirHighpassDR = (iirHighpassDR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassDR;
                iirLowpassDR = (iirLowpassDR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassDR;
                break;
            case 4:
                iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassAL;
                iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassAL;
                iirHighpassBL = (iirHighpassBL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassBL;
                iirLowpassBL = (iirLowpassBL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassBL;
                iirHighpassEL = (iirHighpassEL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassEL;
                iirLowpassEL = (iirLowpassEL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassEL;
                iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassAR;
                iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassAR;
                iirHighpassBR = (iirHighpassBR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassBR;
                iirLowpassBR = (iirLowpassBR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassBR;
                iirHighpassER = (iirHighpassER * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassER;
                iirLowpassER = (iirLowpassER * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassER;
                break;
            case 5:
                iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassAL;
                iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassAL;
                iirHighpassCL = (iirHighpassCL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassCL;
                iirLowpassCL = (iirLowpassCL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassCL;
                iirHighpassFL = (iirHighpassFL * invHighpassL) + (inputSampleL * highpassAmountL); inputSampleL -= iirHighpassFL;
                iirLowpassFL = (iirLowpassFL * invLowpassL) + (inputSampleL * lowpassAmountL); inputSampleL = iirLowpassFL;
                iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassAR;
                iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassAR;
                iirHighpassCR = (iirHighpassCR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassCR;
                iirLowpassCR = (iirLowpassCR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassCR;
                iirHighpassFR = (iirHighpassFR * invHighpassR) + (inputSampleR * highpassAmountR); inputSampleR -= iirHighpassFR;
                iirLowpassFR = (iirLowpassFR * invLowpassR) + (inputSampleR * lowpassAmountR); inputSampleR = iirLowpassFR;
                break;
        }
        //Highpass Filter chunk. This is three poles of IIR highpass, with a 'gearbox' that progressively
        //steepens the filter after minimizing artifacts.

        inputSampleL = (drySampleL * (1.0-wet)) + (inputSampleL * nonLinTrim * wet);
        inputSampleR = (drySampleR * (1.0-wet)) + (inputSampleR * nonLinTrim * wet);

        //begin 32 bit stereo floating point dither
        int expon; frexpf((float)inputSampleL, &expon);
        fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
        inputSampleL += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
        frexpf((float)inputSampleR, &expon);
        fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
        inputSampleR += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
        //end 32 bit stereo floating point dither

        outputs[OUTPUT_1_OUTPUT].setVoltage(inputSampleL);
        outputs[OUTPUT_2_OUTPUT].setVoltage(inputSampleR);
	}
};


struct Capacitor2Widget : ModuleWidget {
	Capacitor2Widget(Capacitor2* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Capacitor2.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.25, 29.742)), module, Capacitor2::LOWPASS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.25, 45.08)), module, Capacitor2::HIGHPASS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.25, 60.507)), module, Capacitor2::NONLIN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.25, 76.421)), module, Capacitor2::DRYWET_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.819, 96.346)), module, Capacitor2::INPUT_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.64, 96.346)), module, Capacitor2::INPUT_2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.682, 113.214)), module, Capacitor2::OUTPUT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.846, 113.333)), module, Capacitor2::OUTPUT_1_OUTPUT));
	}
};


Model* modelCapacitor2 = createModel<Capacitor2, Capacitor2Widget>("Capacitor2");