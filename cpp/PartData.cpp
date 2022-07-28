//
// Created by 光球层上的黑子 on 2022/6/15.
//

#include "../head/PartData.h"

void PartData::setContainer(const std::string &container) {
    PartData::container = container;
}

const std::string &PartData::getContainer() const {
    return container;
}
