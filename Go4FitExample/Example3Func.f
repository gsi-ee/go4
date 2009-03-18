        double precision function GAUSSIAN(axis,pars)

        double precision axis, pars(2)

        GAUSSIAN = exp(-0.5*(axis-pars(1))**2/pars(2)**2)

        return

        end
