#!/bin/bash -e

# This script will create or update icon sets for macOS (icns) and windows (ico) at the same time (if possible).
# This script can be run anywhere bash/imagemagick is installed on (Windows with linux subsystem, unix...) but
# it is preferable to run this tool from macOS as nothing else than a mac can create a icns file...
# Anything bigger than 64 pixels will be script generated from the biggest image.

HAVE_SIPS=0
HAVE_ICONUTIL=0
HAVE_IMAGEMAGICK=0

if [ -x "$(command -v sips)" ]; then
    HAVE_SIPS=1
fi

if [ -x "$(command -v iconutil)" ]; then
    HAVE_ICONUTIL=1
fi

if [ -x "$(command -v convert)" ]; then
    HAVE_IMAGEMAGICK=1
fi

SOURCE_SCALES=(1 2 3)
SOURCE_SIZES=(16 24 32 48 64 128 256 512 1024)
BIGGEST_SOURCE_SIZE=${SOURCE_SIZES[${#SOURCE_SIZES[@]}-1]}
SOURCE_MASK=GammaRay-
TARGET_MASK=icon_
SOURCE_ICONSET=GammaRay.iconset
TARGET_ICNS=GammaRay.icns
TARGET_ICO=GammaRay.ico

mkdir -p "${SOURCE_ICONSET}"

for source_size in "${SOURCE_SIZES[@]}"; do
    if [ ${source_size} -eq ${BIGGEST_SOURCE_SIZE} ]; then
        break;
    fi

    for scale in "${SOURCE_SCALES[@]}"; do
        if [ ${scale} -eq 1 ]; then
            source="${SOURCE_MASK}${source_size}x${source_size}.png"
            target="${SOURCE_ICONSET}/${TARGET_MASK}${source_size}x${source_size}.png"
        else
            source="${SOURCE_MASK}${source_size}x${source_size}@${scale}x.png"
            target="${SOURCE_ICONSET}/${TARGET_MASK}${source_size}x${source_size}@${scale}x.png"
        fi

        # Downscale biggest image into the requested size if needed and possible
        if [ ! -f "${source}" ] && [ ${source_size} -lt ${BIGGEST_SOURCE_SIZE} ]; then
            need=$((source_size*scale))

            if [ ${need} -gt ${BIGGEST_SOURCE_SIZE} ]; then
                echo "- Can not generate '${source}' (${need}x${need}@1x) from ${BIGGEST_SOURCE_SIZE}x${BIGGEST_SOURCE_SIZE}@1x."
            else
                if [ ${HAVE_SIPS} -eq 1 ]; then
                    sips -Z ${source_size} "${SOURCE_MASK}${BIGGEST_SOURCE_SIZE}x${BIGGEST_SOURCE_SIZE}.png" --out "${source}"
                elif [ ${HAVE_IMAGEMAGICK} -eq 1 ]; then
                    convert "${SOURCE_MASK}${BIGGEST_SOURCE_SIZE}x${BIGGEST_SOURCE_SIZE}.png" -resize ${source_size}x${source_size} "${source}"
                else
                    echo "No tool found to do image convertion to ${source}."
                fi
            fi
        fi

        # This is mandatory icons
        if [ -f "${source}" ]; then
            cp -f "${source}" "${target}"
        else
            echo "- Can not find source file ${source}."
        fi
    done
done

if [ ${HAVE_ICONUTIL} -eq 1 ]; then
    iconutil -c icns -o "${TARGET_ICNS}" "${SOURCE_ICONSET}"
fi

if [ ${HAVE_IMAGEMAGICK} -eq 1 ]; then
    find "${SOURCE_ICONSET}" -name '*@*' -exec rm {} \;
    convert "${SOURCE_ICONSET}/"* "${TARGET_ICO}"
fi

rm -r "${SOURCE_ICONSET}"
