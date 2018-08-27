/**
 * \file
 * \brief planesOperations example application
 *
 * \author Copyright (C) 2018 Cezary Gapinski cezary.gapinski@gmail.com
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "distortos/distortosConfiguration.h"

#include "distortos/board/displayOutputs.hpp"

#include "distortos/chip/ChipOutputPin.hpp"

#include "distortos/devices/display/VideoMode.hpp"

#include "distortos/devices/communication/SpiMaster.hpp"
#include "distortos/devices/MipiDbi/MipiDbiTypeC.hpp"
#include "distortos/chip/spis.hpp"
#include "distortos/chip/ChipSpiMasterLowLevel.hpp"

#include "distortos/devices/DisplayDriver/Ili9341.hpp"
#include "distortos/devices/DisplayPanel/SFTC240T9370T.hpp"

#include "distortos/chip/ltdcs.hpp"
#include "distortos/chip/ChipMipiDpiLowLevel.hpp"
#include "distortos/devices/MipiDpi/MipiDpiDevice.hpp"
#include "distortos/devices/display/Framebuffer.hpp"
#include "distortos/devices/display/Plane.hpp"

#include "distortos/ThisThread.hpp"

#include "PrimaryLayerImage.hpp"

#include <array>

distortos::devices::SpiMaster mipiSpiMaster {distortos::chip::spi5};
distortos::devices::MipiDbiTypeC mipiDbiDevice {mipiSpiMaster,
	distortos::board::displayOutputs[distortos::board::displayOutputsspiCsxIndex],
	distortos::board::displayOutputs[distortos::board::displayOutputswrxDcxIndex]};
distortos::devices::Ili9341 ili9341 {mipiDbiDevice};
distortos::devices::SFTC240T9370T sftc240t93t0t {ili9341};
constexpr size_t displayWidth {distortos::devices::SFTC240T9370T::hActive};
constexpr size_t displayHeight {distortos::devices::SFTC240T9370T::vActive};

distortos::devices::MipiDpiDevice rgbDisplay {distortos::chip::ltdc};

distortos::devices::Framebuffer firstPlaneFrameBuffer{distortos::test::primaryPlaneImage,
	distortos::devices::PixelFormat::argb8888, displayWidth, displayHeight};
distortos::devices::Plane displayPrimaryPlane {firstPlaneFrameBuffer, distortos::devices::Plane::Type::primary};

constexpr size_t overlayBufferWidth {128};
constexpr size_t overlayBufferHeight {128};
constexpr size_t overlayBufferSize {overlayBufferWidth*overlayBufferHeight};
constexpr size_t colorLookUpTableSize {16};
std::array<distortos::devices::RgbColor, colorLookUpTableSize> colorLookUpTable;
uint8_t overlayPlaneBuffer[overlayBufferSize];
distortos::devices::Framebuffer overlayPlaneFrameBuffer{overlayPlaneBuffer, distortos::devices::PixelFormat::al44,
	overlayBufferWidth, overlayBufferHeight, distortos::devices::RgbColorsRange{colorLookUpTable}};
distortos::devices::Plane displayOverlayPlane {overlayPlaneFrameBuffer, distortos::devices::Plane::Type::overlay};

std::array<distortos::devices::RgbColor, colorLookUpTableSize> secondColorLookUpTable;
distortos::devices::Framebuffer overlaySecondPlaneFrameBuffer{overlayPlaneBuffer,
	distortos::devices::PixelFormat::al44, overlayBufferWidth, overlayBufferHeight,
	distortos::devices::RgbColorsRange{secondColorLookUpTable}};

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

int main()
{
	// set to high unused read strobe pin
	distortos::board::displayOutputs[distortos::board::displayOutputsrdxIndex].set(true);

	// makes CLUT with colors from yellow to white
	for (size_t i = 0; i < colorLookUpTableSize; i++)
	{
		colorLookUpTable[i].red = 255;
		colorLookUpTable[i].green = 255;
		colorLookUpTable[i].blue = i * 16;

		secondColorLookUpTable[i].red = i * 16;
		secondColorLookUpTable[i].green = 255;
		secondColorLookUpTable[i].blue = 255;
	}

	// creates overlay plane square with transparent field inside in AL44 pixel format
	for (size_t i = 0; i < overlayBufferHeight; i++)
	{
		auto toggle = false;
		for (size_t j = 0; j < overlayBufferWidth; j++)
		{
			if ((j % colorLookUpTable.size()) == 0)
				toggle = !toggle;

			if (toggle)
				overlayPlaneBuffer[i*overlayBufferWidth + j] = 0xf - (j & 0xf);
			else
				overlayPlaneBuffer[i*overlayBufferWidth + j] = j & 0xf;

			overlayPlaneBuffer[i*overlayBufferWidth + j] |= 0xf0;
			// add a opacity and leave inside transparent
			if (((i > 32) && i < (overlayBufferHeight - 32)) && (j > 32 && j < (overlayBufferWidth - 32)))
				overlayPlaneBuffer[i*overlayBufferWidth + j] &= 0x00;
		}
	}

	if (rgbDisplay.open(distortos::devices::SFTC240T9370T::videoMode) != 0)
		while(1);

	if (sftc240t93t0t.init() != 0)
		while(1);

	displayPrimaryPlane.position(0, 0);
	displayPrimaryPlane.enable();

	displayOverlayPlane.position(0, 0);
	displayOverlayPlane.disable();

	distortos::devices::DisplayPlaneOperation operations[]
	{
			distortos::devices::DisplayPlaneOperation{displayPrimaryPlane},
			distortos::devices::DisplayPlaneOperation{displayOverlayPlane}
	};
	distortos::devices::DisplayPlaneOperationsRange displayPlaneOperationsRange{operations};
	if (rgbDisplay.executeTransaction(displayPlaneOperationsRange) != 0)
		while(1);

	distortos::ThisThread::sleepFor(std::chrono::seconds{5});

	int16_t xPos = 0;
	int16_t yPos = 0;
	displayOverlayPlane.enable();

	// moves overlay square to right down corner
	while (1)
	{
		distortos::ThisThread::sleepFor(std::chrono::milliseconds{50});
		displayOverlayPlane.position(xPos, yPos);
		if (rgbDisplay.executeTransaction(displayPlaneOperationsRange) != 0)
		{
			if (yPos == displayHeight - overlayBufferHeight + 2)
			{
				yPos -= 2;
				break;
			}
			else
				while(1);
		}

		xPos += 1;
		yPos += 2;
	}

	// check if V Sync event, to detect update of previous operation during V Sync
	rgbDisplay.waitForVSyncEvent();
	displayOverlayPlane.swapFramebuffer(overlaySecondPlaneFrameBuffer);
	displayOverlayPlane.position(xPos, yPos);

	if (rgbDisplay.executeTransaction(displayPlaneOperationsRange) != 0)
		while(1);

	/// move overlay square to up left corner
	while(1)
	{
		distortos::ThisThread::sleepFor(std::chrono::milliseconds{50});
		displayOverlayPlane.position(xPos, yPos);
		if (rgbDisplay.executeTransaction(displayPlaneOperationsRange) != 0)
		{
			if (yPos == -2)
				break;
			else
				while(1);
		}

		xPos -= 1;
		yPos -= 2;
	}

	while(1);
}
