#ifndef ITECHNICALINSPECTOR_HPP
#define ITECHNICALINSPECTOR_HPP

class Transport;

class ITechnicalInspector {
public:
    virtual ~ITechnicalInspector() = default;
    [[nodiscard]] virtual bool approve(const Transport& transport) const = 0;
};

#endif
